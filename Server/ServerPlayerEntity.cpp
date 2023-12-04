#include "ServerPlayerEntity.h"
#include "ServerWorld.h"
#include <format>

ServerPlayerEntity::ServerPlayerEntity(ConnectedClient* client, EntityID id, sf::Vector2f position, float rotation) : PlayerEntity(id, position, rotation), client{ client } {
	client->player = this;
}

ServerPlayerEntity::~ServerPlayerEntity() {
	if(client->player){
		client->Send(PacketFactory::PlayerSetClientID(std::nullopt));
		client->player = nullptr;
	}
}

std::optional<sf::Vector2f> ServerPlayerEntity::Shoot(ServerWorld* world) {
	auto result = world->RayCast(this, getPosition(), getDirection());
	for (auto& r : result) {
		if(r.entity->GetType() == EntityType::BARRIER) {
			return getPosition() + getDirection() * r.distance;
		}
		if (r.entity->GetType() == EntityType::PLAYER) {
			world->DamagePlayer((ServerPlayerEntity*)r.entity, this, 10);
			return {};
		}
	}
	return {};
}

void ServerPlayerEntity::Update(World* world) {
	if(health == 0) {
		Kill();
	}
}
