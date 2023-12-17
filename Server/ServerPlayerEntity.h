#pragma once
#include "PlayerEntity.h"
#include "PacketFactory.h"
#include "ConnectedClient.h"
#include <map>

class ServerWorld;

class ServerPlayerEntity : public PlayerEntity {
	std::map<int, InputData> inputBuffer;
	int currentInputIndex;
	int lastProcessedInputIndex;
public:
	ServerPlayerEntity(ConnectedClient* client, EntityID id, sf::Vector2f position, float rotation);
	~ServerPlayerEntity();
	void BufferInput(const std::vector<InputData>& inputData);
	PacketFactory::PlayerStateData GetPlayerState() const;
	void Update(World* world) override;
	ConnectedClient* client;
};

