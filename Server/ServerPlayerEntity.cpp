#include "ServerPlayerEntity.h"
#include "ServerWorld.h"
#include <format>
#include <iostream>
#include <limits>
#include <ranges>

const int INPUT_LAG{4};

ServerPlayerEntity::ServerPlayerEntity(ConnectedClient* client, EntityID id, sf::Vector2f position, float rotation) : 
PlayerEntity(id, position, rotation), 
client{ client },
lastProcessedInputIndex{0},
inputIndexOffset{0.01f} {
	inputBuffer[-1000] = {};
	client->player = this;
}

ServerPlayerEntity::~ServerPlayerEntity() {
	if(client->player){
		client->player = nullptr;
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

const PacketFactory::PlayerStateData& ServerPlayerEntity::GetHistoricalPlayerState(int ticksPast) const {
	assert(ticksPast >= 0);
	assert(history.size() > 0);
	if(ticksPast >= history.size()) {
		//Oldest element
		return history[0];
	}else{
		return history[history.size()-ticksPast-1];
	}
}

void ServerPlayerEntity::BufferInput(int tick, InputData inputData) {
	inputBuffer[inputData.index] = inputData;
	inputIndexOffset.AddValue(inputData.index - tick);
}

void ServerPlayerEntity::Update(World* world) {
	if(inputIndexOffset.IsInitialised()){
		int currentInputIndex{world->GetClock().GetTick()+inputIndexOffset.GetAverage()-INPUT_LAG};
		//Prevent going backwards
		if(currentInputIndex < lastProcessedInputIndex) {
			currentInputIndex = lastProcessedInputIndex;
		}
		//Get the current input index closest indexed input before it
		auto element{--inputBuffer.upper_bound(currentInputIndex)};
		PlayerEntity::UpdateFromInput(world, element->second);
		lastProcessedInputIndex = currentInputIndex;
		history.push(GetPlayerState());
		//Remove old inputs from buffer
		std::erase_if(inputBuffer, [=](const auto& pair) {
			const auto& [i, data] = pair;
			return i < element->first;
		});
	}
	//Other Update
	if(health == 0) {
		Kill();
	}
}