#pragma once
#include "World.h"
#include <string>
#include <SFML/Network/TcpListener.hpp>
#include "ServerPlayerEntity.h"

class ServerWorld : public World {
public:
    ServerWorld(unsigned short port);
    void Update();
    bool Listening();
    EntityID GetNewID();
    void Broadcast(sf::Packet& packet, ServerPlayerEntity* excluding = nullptr);
    void Broadcast(const std::string& message);
    void DamagePlayer(ServerPlayerEntity* target, ServerPlayerEntity* source, int amount);
private:
    void ConnectClients(sf::TcpListener& listener);
    void DisconnectClients();
    void Tick(float deltaTime);
    sf::TcpListener listener;
    EntityID usedEntityIds;
    bool listening;
};