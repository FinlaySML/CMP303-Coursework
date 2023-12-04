#include "ServerWorld.h"
#include "PacketFactory.h"
#include "ServerPlayerEntity.h"
#include <iostream>
#include <format>
#include "BarrierEntity.h"
#include "BulletHoleEntity.h"

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
    //Connect clients
    if (auto newClient = networking.GetNewClient()) {
        ServerPlayerEntity* newPlayer{ new ServerPlayerEntity(newClient, GetNewEntityID(), {0.0f, 0.0f}, 0.0f)};
        AddEntity(newPlayer);
        networking.Broadcast(newPlayer->CreationPacket(), newClient->id);
        newClient->Send(PacketFactory::JoinGame(tickClock.GetTick()));
        for (auto& [id, entity] : entities) {
            newClient->Send(entity->CreationPacket());
        }
        newClient->Send(PacketFactory::PlayerSetClientID(newPlayer->GetID()));
        networking.Broadcast(std::format("A player (ID={}) has joined the game", newClient->id));
    }
    // Recieve Packets
    networking.ProcessPackets([&](sf::Packet& packet, ConnectedClient* client){
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
        }
    }
    CleanEntities();
    //Clear clients
    for (auto& client : disconnected) {
        networking.Broadcast(std::format("A player (ID={}) has left the game", client->id));
    }
    // Send Player Update Packets
    for (auto& [id, entity] : entities) {
        if (entity->GetType() == EntityType::PLAYER) {
            networking.Broadcast(entity->UpdatePacket(), 0, false);
        }
    }
}

void ServerWorld::DamagePlayer(ServerPlayerEntity* target, ServerPlayerEntity* source, int amount) {
    target->Damage(amount);
    networking.Broadcast(PacketFactory::PlayerDamage(target->GetID(), amount));
}