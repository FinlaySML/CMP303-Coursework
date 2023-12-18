#pragma once
#include "PlayerEntity.h"
#include "PacketFactory.h"
#include "ConnectedClient.h"
#include "MovingAverage.h"
#include <map>

class ServerWorld;

class ServerPlayerEntity : public PlayerEntity {
	std::map<int, InputData> inputBuffer;
	int lastProcessedInputIndex;
	MovingAverage inputIndexOffset;
public:
	ServerPlayerEntity(ConnectedClient* client, EntityID id, sf::Vector2f position, float rotation);
	~ServerPlayerEntity();
	PacketFactory::PlayerStateData GetPlayerState() const;
	void BufferInput(int tick, InputData inputData);
	void Update(World* world) override;
	ConnectedClient* client;
};

