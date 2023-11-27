#pragma once
#include "PlayerEntity.h"
#include "ConnectedSocket.h"

class ServerWorld;

class ServerPlayerEntity : public PlayerEntity {
public:
	ServerPlayerEntity(ConnectedSocket&& socket, EntityID id, sf::Vector2f position, float rotation);
	void Shoot(ServerWorld* world);
	ConnectedSocket socket;
};

