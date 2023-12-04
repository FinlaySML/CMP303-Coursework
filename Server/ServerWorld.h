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
private:
    void Tick();
    ServerNetworking networking;
    EntityID usedEntityIds;
};