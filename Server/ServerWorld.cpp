#include "ServerWorld.h"
#include "PacketFactory.h"
#include "ServerPlayerEntity.h"
#include <iostream>
#include <format>
#include "BarrierEntity.h"
#include "BulletHoleEntity.h"
#include "RocketEntity.h"
#include <cassert>

const float CLIENT_UPDATE_FREQUENCY{10.0f};

ServerWorld::ServerWorld(unsigned short port) : 
World(World::Side::SERVER), 
usedEntityIds{0}, 
networking{port},
clientUpdateAccumulator{0.0f} {
    //32 wide x 24 tall
    const std::string mapData{
    "################################"
    "#                              #"
    "#                              #"
    "#  #########    ##########  #  #"
    "#  #       #          #     #  #"
    "#  #       #          #     #  #"
    "#  ###  #  ######  #  #  ####  #"
    "#    #  #          #  #     #  #"
    "#    #  #          #  #     #  #"
    "#  ###  ############  ####  #  #"
    "#  #                  #     #  #"
    "#  #                  #     #  #"
    "#  #####  ########  ###  ####  #"
    "#                              #"
    "#                              #"
    "#     ######        #########  #"
    "#     #    #        #       #  #"
    "#     #             #       #  #"
    "#     ######           ###  #  #"
    "#     #             #       #  #"
    "#     #    #        #       #  #"
    "#     ######        #########  #"
    "#                              #"
    "################################"
    };
    for(int y{0}; y < 24; y++) {
        for (int x{0}; x < 32; x++) {
            if(mapData[y*32+x] == '#') {
                AddEntity(new BarrierEntity(GetNewEntityID(), sf::Vector2f(x - 16, y - 12)));
            }
        }
    }
}

EntityID ServerWorld::GetNewEntityID() {
    usedEntityIds++;
    return usedEntityIds;
}

void ServerWorld::Update() {
    bool executedTick = tickClock.ExecuteTick([&]() {
        Tick();
    });
    if (!executedTick) {
        sf::sleep(sf::milliseconds(1));
    }
}

void ServerWorld::Tick() {
    //Update clients
    auto dueRespawn = networking.UpdateRespawnTimer(tickClock.GetTickDelta());
    for(ConnectedClient* client : dueRespawn) {
        SpawnPlayer(client);
    }
    //Connect clients
    ConnectedClient* nc{networking.GetNewClient()};
    if(nc) nc->Send(PacketFactory::SetTick(tickClock.GetTick()));
    // Recieve Packets
    std::vector<ConnectedClient*> clientsToInit;
    networking.ProcessPackets([&](sf::Packet& packet, ConnectedClient* client){
        PacketType type{ PacketFactory::GetType(packet) };
        if (type == PacketType::PLAYER_INPUT) {
            if (client->player) {
                auto data = PacketFactory::PlayerInput(packet);
                client->player->BufferInput(tickClock.GetTick(), data);
            }
        }
        if(type == PacketType::ACK_TICK) {
            client->pingTicks.AddValue(tickClock.GetTick() - PacketFactory::AckTick(packet));
            client->SendUnreliable(PacketFactory::TellRTT(client->pingTicks.GetAverage()));
            if (client->GetStatus() == ConnectedClient::Status::LOADING) {
                clientsToInit.push_back(client);
            }
        }
    });
    for(auto* newClient : clientsToInit) {
        if (newClient->GetStatus() == ConnectedClient::Status::LOADING) {
            // Join game
            for (auto& [id, entity] : entities) {
                newClient->Send(entity->CreationPacket(tickClock.GetTick()));
            }
            networking.Broadcast(std::format("Player {} has joined the game", newClient->id));
            // Spawn player
            SpawnPlayer(newClient);
        }
    }
    //Update Entities (can't iterate directly because if an entity is spawned in Update it would invalidate the iterator)
    auto allEntities{GetEntities()};
    for (Entity* entity : allEntities) {
        entity->Update(this);
    }
    auto disconnected{networking.GetDisconnectedClients()};
    for(auto& client : disconnected) {
        if(client->player) {
            client->player->Kill();
        }
    }
    //Clear entities
    for (auto& [id, entity] : entities) {
        if (entity->ShouldRemove()) {
            networking.Broadcast(PacketFactory::EntityDelete(id));
            if(entity->GetType() == EntityType::PLAYER) {
                auto* serverPlayer{(ServerPlayerEntity*)entity.get()};
                serverPlayer->client->IncrementStat(Stats::Type::DEATHS);
                serverPlayer->client->SetRespawning(10.0f);
                if(auto cause = TryGetEntity(serverPlayer->GetCauseOfDeath(), EntityType::PLAYER)) {
                    ServerPlayerEntity* murderer{ ((ServerPlayerEntity*)cause.value()) };
                    if (murderer != serverPlayer) murderer->client->IncrementStat(Stats::Type::KILLS);
                    networking.Broadcast(std::format("Player {} was killed by player {}", serverPlayer->client->id, murderer->client->id));
                }
            }
        }
    }
    CleanEntities();
    //Clear clients
    for (auto& client : disconnected) {
        networking.Broadcast(std::format("Player {} has left the game", client->id));
    }
    clientUpdateAccumulator += tickClock.GetTickDelta() * CLIENT_UPDATE_FREQUENCY;
    if(clientUpdateAccumulator > 1) {
        clientUpdateAccumulator--;
        // Refresh client clocks
        networking.BroadcastUnreliable(PacketFactory::SetTick(tickClock.GetTick()), 0);
        // Send Player Update Packets
        for (auto& [id, entity] : entities) {
            if (entity->GetType() == EntityType::PLAYER || entity->GetType() == EntityType::ROCKET) {
                networking.BroadcastUnreliable(entity->UpdatePacket(tickClock.GetTick()), 0);
            }
        }
    }
    //Reconcile players
    for (auto& [id, entity] : entities) {
        if (entity->GetType() == EntityType::PLAYER) {
            ServerPlayerEntity* p{(ServerPlayerEntity*)entity.get()};
            p->client->SendUnreliable(PacketFactory::PlayerState(p->GetPlayerState()));
        }
    }
    // Check TCP connection
    if (tickClock.GetTick() % CHECK_CONNECTION_INTERVAL == 0) {
        networking.Broadcast(PacketFactory::None());
    }
}

void ServerWorld::DamagePlayer(ServerPlayerEntity* target, ServerPlayerEntity* source, int amount) {
    target->Damage(source->GetID(), amount);
    networking.Broadcast(PacketFactory::PlayerDamage(target->GetID(), amount));
}

void ServerWorld::SpawnPlayer(ConnectedClient* client) {
    //Create entity
    ServerPlayerEntity* newPlayer{ new ServerPlayerEntity(client, GetNewEntityID(), {0.0f, 0.0f}, 0.0f) };
    AddEntity(newPlayer);
    networking.Broadcast(newPlayer->CreationPacket(tickClock.GetTick()));
    //Send update packeet
    client->SetPlaying(newPlayer);
}

void ServerWorld::GunEffects(EntityID sourceEntity, EntityID hitEntity, sf::Vector2f hitPosition) {
    auto source{ TryGetEntity(sourceEntity, EntityType::PLAYER) };
    auto hit{ TryGetEntity(hitEntity) };
    if(source) {
        auto sourcePlayer{ (ServerPlayerEntity*)source.value() };
        //Play gun sound on other clients
        networking.BroadcastUnreliable(PacketFactory::GunEffects(sourceEntity, hitEntity, hitPosition), sourcePlayer->client->id);
        if(hit) {
            if(hit.value()->GetType() == EntityType::PLAYER) {
                //Damage player
                auto hitPlayer{((ServerPlayerEntity*)hit.value())};
                DamagePlayer(hitPlayer, sourcePlayer, 50);
                //Hit
                sourcePlayer->client->IncrementStat(Stats::Type::HITS);
            }else{
                //Create bullet hole
                BulletHoleEntity* bulletHole = new BulletHoleEntity(GetNewEntityID(), hitPosition, 0, tickClock.GetTick(), 1200);
                AddEntity(bulletHole);
                networking.Broadcast(bulletHole->CreationPacket(tickClock.GetTick()));
                //Miss
                sourcePlayer->client->IncrementStat(Stats::Type::MISSES);
            }
        }
    }
}

void ServerWorld::FireRocket(EntityID sourceEntity, sf::Vector2f position, float rotation, int lifetime) {
    RocketEntity* rocket = new RocketEntity(GetNewEntityID(), position, rotation, sourceEntity, tickClock.GetTick(), lifetime);
    AddEntity(rocket);
    networking.Broadcast(rocket->CreationPacket(tickClock.GetTick()));
}

bool ServerWorld::TryExplodeRocket(EntityID sourceEntity, EntityID rocketEntity, sf::Vector2f point) {
    for(auto& [id, entity] : entities) {
        if (id == sourceEntity) continue;
        if (id == rocketEntity) continue;
        if (entity->GetType() == EntityType::BULLET_HOLE) continue;
        if (entity->GetType() == EntityType::UNKNOWN) continue;
        if (!entity->ContainsPoint(point)) continue;
        // Explode
        for (auto& [id, entity] : entities) {
            if(entity->GetType() != EntityType::PLAYER) continue;
            // Get vector to player
            sf::Vector2f d{ entity->getPosition() - point };
            float length{ sqrtf(d.x * d.x + d.y * d.y) };
            // Get damage dealt if hit
            float power{ 1 - length / 6 };
            int damage{ (int)(std::clamp(power, 0.0f, 1.0f) * 2000.0f) };
            if(damage < 10) continue;// Minimum damage threshold
            d /= length;
            // Cast ray
            auto result{ RayCast(rocketEntity, point + d * 0.5f, d, length, 0.1f, 0) };
            for(auto& [hitEntity, distance] : result) {
                if(hitEntity->GetType() == EntityType::BARRIER) break;
                if(hitEntity->GetID() != entity->GetID()) continue;
                DamagePlayer((ServerPlayerEntity*)hitEntity, (ServerPlayerEntity*)TryGetEntity(sourceEntity).value_or(nullptr), damage);
            }
        }
        return true;
    }
    return false;
}
