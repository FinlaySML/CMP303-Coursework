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
currentInputIndex{ 0 } {
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
//Possibilities
//1. Dropped input -> clone the input from before; progress index
//2. Too far ahead (no inputs) -> clone input from before; don't progressing index
//3. Too far behind (buffer to large) -> remove inputs from the front of the buffer; progress index;

void ServerPlayerEntity::Update(World* world) {
	//Get min and max index
	auto bounds = std::ranges::minmax(std::views::keys(inputBuffer));
	//Remove excess slack from buffer
	currentInputIndex = std::max(currentInputIndex, bounds.max - MAX_BUFFER_SIZE + 1);
	int tempIndex;
	if(currentInputIndex > bounds.max) {
		tempIndex = bounds.max;
	}else {
		tempIndex = currentInputIndex;
		while (!inputBuffer.contains(tempIndex)) tempIndex--;
		currentInputIndex++;
	}
	PlayerEntity::UpdateFromInput(world, inputBuffer[tempIndex]);
	std::erase_if(inputBuffer, [=](const auto& pair) {
		const auto& [i, data] = pair;
		return i < tempIndex;
	});
	if(health == 0) {
		Kill();
	}
}