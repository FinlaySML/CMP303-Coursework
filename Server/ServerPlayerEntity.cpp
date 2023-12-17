#include "ServerPlayerEntity.h"
#include "ServerWorld.h"
#include <format>
#include <iostream>
#include <limits>
#include <ranges>

const int MAX_BUFFER_SIZE{4};

ServerPlayerEntity::ServerPlayerEntity(ConnectedClient* client, EntityID id, sf::Vector2f position, float rotation) : 
PlayerEntity(id, position, rotation), 
client{ client },
currentInputIndex{ 0 },
lastProcessedInputIndex{0} {
	client->player = this;
	inputBuffer[0] = {};
}

ServerPlayerEntity::~ServerPlayerEntity() {
	if(client->player){
		client->player = nullptr;
	}
}

void ServerPlayerEntity::BufferInput(const std::vector<InputData>& inputData) {
	for(const auto& d : inputData) {
		inputBuffer[d.index] = d;
	}
}
PacketFactory::PlayerStateData ServerPlayerEntity::GetPlayerState() const {
	PacketFactory::PlayerStateData state;
	state.position = getPosition();
	state.rotation = getRotation();
	state.gunCooldown = gunCooldown;
	state.index = lastProcessedInputIndex;
	return state;
}

//Possibilities
//1. Dropped input -> clone the input from before; progress index
//2. Too few inputs in buffer -> clone previous input; don't progress index
//3. Too many inputs in buffer -> remove inputs from the front of the buffer; progress index;

void ServerPlayerEntity::Update(World* world) {
	//Get min and max index
	auto bounds = std::ranges::minmax(std::views::keys(inputBuffer));
	//Remove excess slack from buffer
	currentInputIndex = std::max(currentInputIndex, bounds.max - MAX_BUFFER_SIZE + 1);
	//
	auto element{--inputBuffer.upper_bound(currentInputIndex)};
	PlayerEntity::UpdateFromInput(world, element->second);
	lastProcessedInputIndex = currentInputIndex;
	if (currentInputIndex <= bounds.max) {
		currentInputIndex++;
	}
	//Remove old inputs from buffer
	std::erase_if(inputBuffer, [=](const auto& pair) {
		const auto& [i, data] = pair;
		return i < element->first;
	});
	//Other Update
	if(health == 0) {
		Kill();
	}
}