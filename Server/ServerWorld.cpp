#include "ServerWorld.h"
#include "PacketFactory.h"
#include "ServerPlayerEntity.h"
#include <iostream>
#include <format>
#include "BarrierEntity.h"
#include "BulletHoleEntity.h"
#include <cassert>

const float CLIENT_UPDATE_DELTA = 0.1f;

ServerWorld::ServerWorld(unsigned short port) : usedEntityIds{0}, networking{port} {
    for (int i = 0; i < 16; i++) {
        AddEntity(new BarrierEntity(GetNewEntityID(), sf::Vector2f(i - 8, -6)));
        AddEntity(new BarrierEntity(GetNewEntityID(), sf::Vector2f(i - 8, 5)));
    }
    for (int i = 0; i < 10; i++) {
        AddEntity(new BarrierEntity(GetNewEntityID(), sf::Vector2f(-8, i - 5)));
        AddEntity(new BarrierEntity(GetNewEntityID(), sf::Vector2f(7, i - 5)));
    }
    for (int i = 0; i < 6; i++) {
        AddEntity(new BarrierEntity(GetNewEntityID(), sf::Vector2f(-5, i - 3)));
        AddEntity(new BarrierEntity(GetNewEntityID(), sf::Vector2f(4, i - 3)));
    }
    for (int i = 0; i < 6; i++) {
        AddEntity(new BarrierEntity(GetNewEntityID(), sf::Vector2f(-i + 1, 2)));
        AddEntity(new BarrierEntity(GetNewEntityID(), sf::Vector2f(i - 2, -3)));
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
    networking.GetNewClient();
    // Recieve Packets
    std::vector<ConnectedClient*> clientsToInit;
    networking.ProcessPackets([&](sf::Packet& packet, ConnectedClient* client){
        PacketType type{ PacketFactory::GetType(packet) };
        if (type == PacketType::PING) {
            client->Send(PacketFactory::Pong(tickClock.GetTick()));
            clientsToInit.push_back(client);
        }
        if (type == PacketType::PLAYER_INPUT) {
            if (client->player) {
                PlayerEntity::InputData data = PacketFactory::PlayerInput(packet);
                client->player->UpdateWithInput(tickClock.GetTickDelta(), data);
                client->player->Collision(this);
            }
        }
        if (type == PacketType::PLAYER_SHOOT) {
            if (client->player) {
                std::optional<sf::Vector2f> shootReturn{ client->player->Shoot(this) };
                sf::Packet gunEffectsPacket{ PacketFactory::GunEffects() };
                networking.Broadcast(gunEffectsPacket, client->id);//Client already played the sound on their end so they should be excluded
                if (shootReturn) {
                    BulletHoleEntity* bulletHole = new BulletHoleEntity(GetNewEntityID(), shootReturn.value(), 0, tickClock.GetTick(), 1200);
                    AddEntity(bulletHole);
                    networking.Broadcast(bulletHole->CreationPacket());
                }
            }
        }
    });
    for(auto* newClient : clientsToInit) {
        if(newClient->GetStatus() == ConnectedClient::Status::LOADING) {
            //Join game
            for (auto& [id, entity] : entities) {
                newClient->Send(entity->CreationPacket());
            }
            networking.Broadcast(std::format("A player (ID={}) has joined the game", newClient->id));
            //Spawn player
            SpawnPlayer(newClient);
        }
    }
    //Update Entities
    for (auto& [id, entity] : entities) {
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
                    ((ServerPlayerEntity*)cause.value())->client->IncrementStat(Stats::Type::KILLS);
                }
            }
        }
    }
    CleanEntities();
    //Clear clients
    for (auto& client : disconnected) {
        networking.Broadcast(std::format("A player (ID={}) has left the game", client->id));
    }
    clientUpdateAccumulator += tickClock.GetTickDelta();
    if(clientUpdateAccumulator > CLIENT_UPDATE_DELTA) {
        clientUpdateAccumulator -= CLIENT_UPDATE_DELTA;
        // Send the status of the client (respawning, playing, or loading)
        networking.Broadcast(PacketFactory::None());
        // Send Player Update Packets
        for (auto& [id, entity] : entities) {
            if (entity->GetType() == EntityType::PLAYER) {
                networking.Broadcast(entity->UpdatePacket(), 0, false);
            }
        }
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
    networking.Broadcast(newPlayer->CreationPacket());
    //Send update packeet
    client->SetPlaying(newPlayer);
}
