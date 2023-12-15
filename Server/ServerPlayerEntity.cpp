#include "ServerPlayerEntity.h"
#include "ServerWorld.h"
#include <format>

ServerPlayerEntity::ServerPlayerEntity(ConnectedClient* client, EntityID id, sf::Vector2f position, float rotation) : 
PlayerEntity(id, position, rotation), 
client{ client } {
	client->player = this;
}

ServerPlayerEntity::~ServerPlayerEntity() {
	if(client->player){
		client->player = nullptr;
	}
}

std::optional<sf::Vector2f> ServerPlayerEntity::Shoot(ServerWorld* world) {
	auto result = world->RayCast(this, getPosition(), getDirection());
	for (auto& r : result) {
		if(r.entity->GetType() == EntityType::BARRIER) {
			client->IncrementStat(Stats::Type::MISSES);
			return getPosition() + getDirection() * r.distance;
		}
		if (r.entity->GetType() == EntityType::PLAYER) {
			client->IncrementStat(Stats::Type::HITS);
			world->DamagePlayer((ServerPlayerEntity*)r.entity, this, 50);
			return {};
		}
	}
	client->IncrementStat(Stats::Type::MISSES);
	return {};
}

void ServerPlayerEntity::Update(World* world) {
	if(health == 0) {
		Kill();
	}
}
