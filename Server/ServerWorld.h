#pragma once
#include "World.h"
#include <string>
#include <SFML/Network/TcpListener.hpp>
#include "ServerPlayerEntity.h"
#include "ConnectedClient.h"

class ServerWorld : public World {
public:
    ServerWorld(unsigned short port);
    void Update();
    bool Listening() const;
    EntityID GetNewEntityID();
    ClientID GetNewClientID();
    void Broadcast(sf::Packet&& packet, ClientID excluding = 0, bool reliable = true);
    void Broadcast(sf::Packet& packet, ClientID excluding = 0, bool reliable = true);
    void Broadcast(const std::string& message, ClientID excluding = 0, bool reliable = true);
    void DamagePlayer(ServerPlayerEntity* target, ServerPlayerEntity* source, int amount);
private:
    void Tick();
    sf::TcpListener listener;
    std::vector<std::unique_ptr<ConnectedClient>> clients;
    EntityID usedEntityIds;
    ClientID usedClientIds;
    bool listening;
};