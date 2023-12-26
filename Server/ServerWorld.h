#pragma once
#include "World.h"
#include <string>
#include <SFML/Network/TcpListener.hpp>
#include "ServerPlayerEntity.h"
#include "ServerNetworking.h"

class ServerWorld : public World {
public:
    ServerWorld(unsigned short port);
    void Update();
    EntityID GetNewEntityID();
    void DamagePlayer(ServerPlayerEntity* target, ServerPlayerEntity* source, int amount);
    void SpawnPlayer(ConnectedClient* client);
    void GunEffects(EntityID sourceEntity, EntityID hitEntity, sf::Vector2f hitPosition) override;
    void FireRocket(EntityID sourceEntity, sf::Vector2f position, float rotation, int lifetime) override;
    bool TryExplodeRocket(EntityID sourceEntity, EntityID rocketEntity, sf::Vector2f point) override;
private:
    void Tick();
    ServerNetworking networking;
    EntityID usedEntityIds;
    float clientUpdateAccumulator;
};