#pragma once
#include "PlayerEntity.h"
#include "PacketFactory.h"
#include "ConnectedClient.h"

class ServerWorld;

class ServerPlayerEntity : public PlayerEntity {
public:
	ServerPlayerEntity(ConnectedClient* client, EntityID id, sf::Vector2f position, float rotation);
	~ServerPlayerEntity();
	std::optional<sf::Vector2f> Shoot(ServerWorld* world);
	void Update(World* world) override;
	ConnectedClient* client;
};

