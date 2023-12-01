#include "ServerPlayerEntity.h"
#include "ServerWorld.h"


ServerPlayerEntity::ServerPlayerEntity(ConnectedSocket&& socket, EntityID id, sf::Vector2f position, float rotation) : PlayerEntity(id, position, rotation), socket{ std::move(socket) } {}

PacketFactory::GunEffectData ServerPlayerEntity::Shoot(ServerWorld* world) {
	auto result = world->RayCast(this, getPosition(), getDirection());
	for (auto& r : result) {
		if(r.entity->GetType() == EntityType::BARRIER) {
			PacketFactory::GunEffectData data;
			data.bulletHole = getPosition() + getDirection() * r.distance;
			return data;
		}
		if (r.entity->GetType() == EntityType::PLAYER) {
			world->DamagePlayer((ServerPlayerEntity*)r.entity, this, 10);
			return {};
		}
	}
	return {};
}
