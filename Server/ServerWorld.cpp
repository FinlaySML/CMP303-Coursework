#include "ServerWorld.h"
#include "PacketFactory.h"
#include "ServerPlayerEntity.h"
#include <iostream>
#include <format>

//#define DEBUG_CLONE

ServerWorld::ServerWorld() : usedEntityIds{0} {}

EntityID ServerWorld::GetNewID() {
    usedEntityIds++;
    return usedEntityIds - 1;
}

void ServerWorld::Broadcast(const std::string& message) {
    sf::Packet joinMessage{ PacketFactory::Message(message) };
    for (auto& [id, entity] : entities) {
        if(entity->GetType() == EntityType::PLAYER) {
            ServerPlayerEntity* player{(ServerPlayerEntity*)entity.get()};
            player->socket.Send(joinMessage);
        }
    }
    std::cout << message << std::endl;
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
    sf::Packet packet{ PacketFactory::JoinGame(newPlayer->GetID(), {0.0f, 0.0f}, 0.0f, barriers)};
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
                    serverPlayer->Update(deltaTime, data);
                    serverPlayer->Collision(this);
                }
                if (type == PacketFactory::PacketType::PLAYER_SHOOT) {
                    serverPlayer->Shoot(this);
                }
            });
        }
    }
    // Send Movement Packets
    std::vector<PacketFactory::PlayerUpdateData> updateData;
    for (auto& [id, entity] : entities) {
        if (entity->GetType() == EntityType::PLAYER) {
#ifdef DEBUG_CLONE
            if (entity->GetID() == 76) {
                updateData.push_back({ 999, -entity->getPosition(), entity->getRotation() + 180.0f });
                continue;
            }
#endif
            updateData.push_back({ entity->GetID(), entity->getPosition(), entity->getRotation()});
        }
    }
    sf::Packet packet{ PacketFactory::PlayerUpdate(updateData) };
    for (auto& [id, entity] : entities) {
        if (entity->GetType() == EntityType::PLAYER) {
            ServerPlayerEntity* serverPlayer{ (ServerPlayerEntity*)entity.get() };
            serverPlayer->socket.Send(packet);
        }
    }
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
