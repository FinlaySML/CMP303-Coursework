#include "ServerWorld.h"
#include "PacketFactory.h"
#include "ServerPlayerEntity.h"
#include <iostream>
#include <format>
#include "BarrierEntity.h"

ServerWorld::ServerWorld(unsigned short port) : usedEntityIds{0}, listening{false} {
    listener.setBlocking(false);
    if (listener.listen(DEFAULT_PORT) == sf::Socket::Error) {
        std::cout << "Failed to listen to port " << DEFAULT_PORT << std::endl;
    }else{
        listening = true;
    }
    for (int i = 0; i < 16; i++) {
        AddEntity(new BarrierEntity(GetNewID(), sf::Vector2f(i - 8, -6)));
        AddEntity(new BarrierEntity(GetNewID(), sf::Vector2f(i - 8, 5)));
    }
    for (int i = 0; i < 10; i++) {
        AddEntity(new BarrierEntity(GetNewID(), sf::Vector2f(-8, i - 5)));
        AddEntity(new BarrierEntity(GetNewID(), sf::Vector2f(7, i - 5)));
    }
    for (int i = 0; i < 6; i++) {
        AddEntity(new BarrierEntity(GetNewID(), sf::Vector2f(-5, i - 3)));
        AddEntity(new BarrierEntity(GetNewID(), sf::Vector2f(4, i - 3)));
    }
    for (int i = 0; i < 6; i++) {
        AddEntity(new BarrierEntity(GetNewID(), sf::Vector2f(-i + 1, 2)));
        AddEntity(new BarrierEntity(GetNewID(), sf::Vector2f(i - 2, -3)));
    }
}

EntityID ServerWorld::GetNewID() {
    usedEntityIds++;
    return usedEntityIds - 1;
}

void ServerWorld::Broadcast(const std::string& message) {
    sf::Packet joinMessage{ PacketFactory::Message(message) };
    Broadcast(joinMessage);
    std::cout << message << std::endl;
}

void ServerWorld::Broadcast(sf::Packet& packet, ServerPlayerEntity* excluding) {
    for (auto& [id, entity] : entities) {
        if (entity->GetType() == EntityType::PLAYER) {
            ServerPlayerEntity* serverPlayer{ (ServerPlayerEntity*)entity.get() };
            if(serverPlayer != excluding) {
                serverPlayer->socket.Send(packet);
            }
        }
    }
}

void ServerWorld::Update() {
    bool executedTick = tickClock.ExecuteTick([&]() {
        DisconnectClients();
        ConnectClients(listener);
        Tick(tickClock.GetTickDelta());
    });
    if (!executedTick) {
        sf::sleep(sf::milliseconds(1));
    }
}

void ServerWorld::ConnectClients(sf::TcpListener& listener) {
    ConnectedSocket socket{ listener };
    if (!socket.IsConnected()) {
        return;
    }
    ServerPlayerEntity* newPlayer{new ServerPlayerEntity(std::move(socket), GetNewID(), {0.0f, 0.0f}, 0.0f)};
    AddEntity(newPlayer);
    std::vector<PacketFactory::JoinGameData::BarrierData> barriers;
    for(auto& [id, entity] : entities) {
        if(entity->GetType() == EntityType::BARRIER) {
            barriers.push_back({id, entity->getPosition()});
        }
    }
    sf::Packet packet{ PacketFactory::JoinGame(tickClock.GetTick(), newPlayer->GetID(), {0.0f, 0.0f}, 0.0f, barriers)};
    newPlayer->socket.Send(packet);
    Broadcast(std::format("A new player ({}) has joined the game", newPlayer->GetID()));
}

void ServerWorld::DisconnectClients() {
    for (auto& [id, entity] : entities) {
        if(entity->GetType() == EntityType::PLAYER) {
            ServerPlayerEntity* serverPlayer{(ServerPlayerEntity*)entity.get()};
            if (!serverPlayer->socket.IsConnected()) {
                EntityID copyID{serverPlayer->GetID()};
                RemoveEntity(copyID);
                Broadcast(std::format("A player ({}) has disconnected from the game", copyID));
                break;
            }
        }
    }
}

void ServerWorld::Tick(float deltaTime) {
    // Recieve Packets
    for (auto& [id, entity] : entities) {
        if (entity->GetType() == EntityType::PLAYER) {
            ServerPlayerEntity* serverPlayer{ (ServerPlayerEntity*)entity.get() };
            serverPlayer->socket.ProcessPackets([&](sf::Packet& packet) {
                PacketFactory::PacketType type{ PacketFactory::GetType(packet) };
                if (type == PacketFactory::PacketType::PLAYER_INPUT) {
                    PlayerEntity::InputData data = PacketFactory::PlayerInput(packet);
                    serverPlayer->UpdateWithInput(deltaTime, data);
                    serverPlayer->Collision(this);
                }
                if (type == PacketFactory::PacketType::PLAYER_SHOOT) {
                    auto gunEffectData{ serverPlayer->Shoot(this) };
                    sf::Packet gunEffectsPacket{PacketFactory::GunEffects(gunEffectData)};
                    Broadcast(gunEffectsPacket, serverPlayer);
                }
            });
        }
    }
    // Send Movement Packets
    std::vector<PacketFactory::PlayerUpdateData> updateData;
    for (auto& [id, entity] : entities) {
        if (entity->GetType() == EntityType::PLAYER) {
            updateData.push_back({ entity->GetID(), entity->getPosition(), entity->getRotation()});
        }
    }
    sf::Packet packet{ PacketFactory::PlayerUpdate(updateData) };
    Broadcast(packet);
}

void ServerWorld::DamagePlayer(ServerPlayerEntity* target, ServerPlayerEntity* source, int amount) {
    target->Damage(amount);
    sf::Packet packet{PacketFactory::PlayerDamage(target->GetID(), amount)};
    for(auto& [id, entity] : entities) {
        if(entity->GetType() == EntityType::PLAYER) {
            ServerPlayerEntity* serverPlayer{(ServerPlayerEntity*)entity.get()};
            serverPlayer->socket.Send(packet);
        }
    }
}

bool ServerWorld::Listening() {
    return listening;
}
