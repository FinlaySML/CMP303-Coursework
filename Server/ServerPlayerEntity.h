#pragma once
#include "PlayerEntity.h"
#include "PacketFactory.h"
#include "ConnectedClient.h"
#include "MovingAverage.h"
#include <map>
#include "RingBuffer.h"

class ServerWorld;

class ServerPlayerEntity : public PlayerEntity {
	std::map<int, InputData> inputBuffer;
	int lastProcessedInputIndex;
	MovingAverage inputIndexOffset;
	RingBuffer<PacketFactory::PlayerStateData, 60> history;
public:
	ServerPlayerEntity(ConnectedClient* client, EntityID id, sf::Vector2f position, float rotation);
	~ServerPlayerEntity();
	PacketFactory::PlayerStateData GetPlayerState() const;
	const PacketFactory::PlayerStateData& GetHistoricalPlayerState(int ticksPast) const;
	bool ContainsPoint(sf::Vector2f point, int ticksPast = 0) const override;
	void BufferInput(int tick, InputData inputData);
	void Update(World* world) override;
	ConnectedClient* client;
};

