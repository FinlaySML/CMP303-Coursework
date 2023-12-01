#pragma once
#include "PlayerEntity.h"
#include "ConnectedSocket.h"
#include "PacketFactory.h"

class ServerWorld;

class ServerPlayerEntity : public PlayerEntity {
public:
	ServerPlayerEntity(ConnectedSocket&& socket, EntityID id, sf::Vector2f position, float rotation);
	PacketFactory::GunEffectData Shoot(ServerWorld* world);
	ConnectedSocket socket;
};

