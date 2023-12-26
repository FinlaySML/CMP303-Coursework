#include "ServerPlayerEntity.h"
#include "ServerWorld.h"
#include <format>
#include <iostream>
#include <limits>
#include <ranges>

const int INPUT_LAG{4};

const int CLIENT_INTERPOLATION_DELAY{6}; //100 ms = 6 ticks

ServerPlayerEntity::ServerPlayerEntity(ConnectedClient* client, EntityID id, sf::Vector2f position, float rotation) : 
PlayerEntity(id, position, rotation), 
client{ client },
lastProcessedInputIndex{0},
inputIndexOffset{0.1f} {
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
	state.rocketCooldown = rocketCooldown;
	state.index = lastProcessedInputIndex;
	return state;
}

PacketFactory::PlayerStateData ServerPlayerEntity::GetHistoricalPlayerState(int ticksPast) const {
	assert(ticksPast >= 0);
	if(history.size() == 0) {
		return GetPlayerState();
	}
	if(ticksPast >= history.size()) {
		//Oldest element
		return history[0];
	}else{
		return history[history.size()-ticksPast-1];
	}
}

bool ServerPlayerEntity::ContainsPoint(sf::Vector2f point, int ticksPast) const {
	sf::Vector2f diff{ GetHistoricalPlayerState(ticksPast).position - point};
	return diff.x * diff.x + diff.y * diff.y < 0.4f * 0.4f;
}

void ServerPlayerEntity::BufferInput(int tick, InputData inputData) {
	inputBuffer[inputData.index] = inputData;
	inputIndexOffset.AddValue(inputData.index - tick);
}

void ServerPlayerEntity::Update(World* world) {
	if(inputIndexOffset.IsInitialised()){
		int currentInputIndex{ world->GetClock().GetTick() + inputIndexOffset.GetAverage() - INPUT_LAG};
		//Prevent going backwards
		if(currentInputIndex < lastProcessedInputIndex) {
			currentInputIndex = lastProcessedInputIndex;
		}
		//Get the current input index, if not available get the last one that comes before it
		auto element{--inputBuffer.upper_bound(currentInputIndex)};
		PlayerEntity::UpdateFromInput(world, element->second, true, INPUT_LAG + CLIENT_INTERPOLATION_DELAY + client->pingTicks.GetAverage());
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