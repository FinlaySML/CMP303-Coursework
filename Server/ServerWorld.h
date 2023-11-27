#pragma once
#include "World.h"
#include <string>
#include <SFML/Network/TcpListener.hpp>
#include "ServerPlayerEntity.h"

class ServerWorld : public World {
public:
    ServerWorld();
    EntityID GetNewID();
    void Broadcast(const std::string& message);
    void ConnectClients(sf::TcpListener& listener);
    void DisconnectClients();
    void Tick(float deltaTime);
    void DamagePlayer(ServerPlayerEntity* target, ServerPlayerEntity* source, int amount);
private:
    EntityID usedEntityIds;
};

