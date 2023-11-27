#include "ServerPlayerEntity.h"
#include "ServerWorld.h"

ServerPlayerEntity::ServerPlayerEntity(ConnectedSocket&& socket, EntityID id, sf::Vector2f position, float rotation) : PlayerEntity(id, position, rotation), socket{ std::move(socket) } {}

void ServerPlayerEntity::Shoot(ServerWorld* world) {
	auto result = world->RayCast(this, getPosition(), getDirection());
	for (auto& r : result) {
		if(r.entity->GetType() == EntityType::BARRIER) {
			break;
		}
		if (r.entity->GetType() == EntityType::PLAYER) {
			world->DamagePlayer((ServerPlayerEntity*)r.entity, this, 10);
		}
	}
}
