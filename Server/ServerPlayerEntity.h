#pragma once
#include "PlayerEntity.h"
#include "PacketFactory.h"
#include "ConnectedClient.h"
#include <unordered_map>

class ServerWorld;

class ServerPlayerEntity : public PlayerEntity {
	std::unordered_map<int, InputData> inputBuffer;
public:
	ServerPlayerEntity(ConnectedClient* client, EntityID id, sf::Vector2f position, float rotation);
	~ServerPlayerEntity();
	void BufferInput(const std::vector<InputData>& inputData);
	void Update(World* world) override;
	ConnectedClient* client;
	int currentInputIndex;
};

