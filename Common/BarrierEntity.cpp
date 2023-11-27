#include "BarrierEntity.h"

BarrierEntity::BarrierEntity(EntityID id, sf::Vector2f position) : Entity(EntityType::BARRIER, id, position) {}

sf::FloatRect BarrierEntity::GetCollisionBox() const {
	return {sf::Vector2f(getPosition()), sf::Vector2f(1, 1)};
}
