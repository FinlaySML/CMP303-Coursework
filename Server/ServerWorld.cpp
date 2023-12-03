#include "ServerWorld.h"
#include "PacketFactory.h"
#include "ServerPlayerEntity.h"
#include <iostream>
#include <format>
#include "BarrierEntity.h"
#include "BulletHoleEntity.h"

ServerWorld::ServerWorld(unsigned short port) : World(port), usedEntityIds{0}, usedClientIds{0}, listening{false} {
    listener.setBlocking(false);
    if (listener.listen(DEFAULT_PORT) == sf::Socket::Error) {
        std::cout << "Failed to listen to port " << DEFAULT_PORT << std::endl;
    }else{
        listening = true;
    }
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

ClientID ServerWorld::GetNewClientID() {
    usedClientIds++;
    return usedClientIds;
}

void ServerWorld::Broadcast(sf::Packet&& packet, ClientID excluding, bool reliable) {
    sf::Packet p{packet};
    Broadcast(p, excluding, reliable);
}

void ServerWorld::Broadcast(const std::string& message, ClientID excluding, bool reliable) {
    sf::Packet joinMessage{ PacketFactory::Message(message) };
    Broadcast(joinMessage, excluding, reliable);
    std::cout << message << std::endl;
}

void ServerWorld::Broadcast(sf::Packet& packet, ClientID excluding, bool reliable) {
    for (auto& client : clients) {
        if(excluding == client->id) continue;
        client->socket.Send(packet, reliable);
    }
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
    //Connect clients
    if (ConnectedSocket socket{listener, &udp}; socket.IsConnected()) {
        ConnectedClient* newClient{ new ConnectedClient(std::move(socket), GetNewClientID()) };
        clients.emplace_back(newClient);
        ServerPlayerEntity* newPlayer{ new ServerPlayerEntity(newClient, GetNewEntityID(), {0.0f, 0.0f}, 0.0f)};
        AddEntity(newPlayer);
        Broadcast(newPlayer->CreationPacket(), newClient->id);
        newClient->socket.Send(PacketFactory::JoinGame(tickClock.GetTick()));
        for (auto& [id, entity] : entities) {
            newClient->socket.Send(entity->CreationPacket());
        }
        newClient->socket.Send(PacketFactory::PlayerSetClientID(newPlayer->GetID()));
        Broadcast(std::format("A player (ID={}) has joined the game", newClient->id));
    }
    // Recieve Packets
    for (auto& client : clients) {
        client->socket.ProcessPackets([&](sf::Packet& packet) {
            PacketType type{ PacketFactory::GetType(packet) };
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
                    sf::Packet gunEffectsPacket{PacketFactory::GunEffects()};
                    Broadcast(gunEffectsPacket, client->id);//Client already played the sound on their end so they should be excluded
                    if(shootReturn) {
                        BulletHoleEntity* bulletHole = new BulletHoleEntity(GetNewEntityID(), shootReturn.value(), 0, tickClock.GetTick(), 1200);
                        AddEntity(bulletHole);
                        Broadcast(bulletHole->CreationPacket());
                    }
                }
            }
        });
    }
    //Update Entities
    for (auto& [id, entity] : entities) {
        entity->Update(this);
    }
    for(auto& client : clients) {
        if(!client->socket.IsConnected() && client->player) {
            client->player->Kill();
        }
    }
    //Clear entities
    for (auto& [id, entity] : entities) {
        if (entity->ShouldRemove()) {
            Broadcast(PacketFactory::EntityDelete(id));
        }
    }
    CleanEntities();
    //Clear clients
    std::erase_if(clients, [&](const auto& client) {
        bool erase{ !client->socket.IsConnected() };
        if(erase) {
            Broadcast(std::format("A player (ID={}) has left the game", client->id));
        }
        return erase;
    });
    // Send Player Update Packets
    for (auto& [id, entity] : entities) {
        if (entity->GetType() == EntityType::PLAYER) {
            Broadcast(entity->UpdatePacket(), 0, false);
        }
    }
}

void ServerWorld::DamagePlayer(ServerPlayerEntity* target, ServerPlayerEntity* source, int amount) {
    target->Damage(amount);
    Broadcast(PacketFactory::PlayerDamage(target->GetID(), amount));
}

bool ServerWorld::Listening() const {
    return listening;
}
