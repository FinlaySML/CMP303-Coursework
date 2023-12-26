#include "RocketEntity.h"
#include "World.h"

RocketEntity::RocketEntity(EntityID id, sf::Vector2f position, float rotation, EntityID sourceEntity, int creationTick, int despawnTime) :
Entity(EntityType::ROCKET, id, position, rotation),
sourceEntity{sourceEntity},
creationTick{creationTick},
despawnTime{despawnTime}
{
}

sf::Packet RocketEntity::CreationPacket(int tick) const {
	sf::Packet packet{ Entity::CreationPacket(tick) };
	packet << sourceEntity;
	packet << creationTick;
	packet << despawnTime;
	return packet;
}

void RocketEntity::Update(World* world) {
	// Move
	move(getDirection() * world->GetClock().GetTickDelta() * 8.0f);
	// Explode
	if (world->TryExplodeRocket(sourceEntity, GetID(), getPosition())) {
		Kill();
	}
	// Despawn
	if (world->GetClock().GetTick() > creationTick + despawnTime) {
		Kill();
	}
}
