#include "BulletHoleEntity.h"
#include "World.h"

BulletHoleEntity::BulletHoleEntity(EntityID id, sf::Vector2f position, float rotation, int creationTick, int despawnTime, sf::Color color) :
Entity(EntityType::BULLET_HOLE, id, position, rotation),
creationTick{creationTick},
despawnTime{despawnTime},
color{color} {}

sf::Packet BulletHoleEntity::CreationPacket(int tick) const {
	sf::Packet packet{Entity::CreationPacket(tick)};
	packet << creationTick;
	packet << despawnTime;
	packet << color.r << color.g << color.b << color.a;
	return packet;
}

void BulletHoleEntity::Update(World* world) {
	if(world->GetClock().GetTick() > creationTick + despawnTime) Kill();
}
