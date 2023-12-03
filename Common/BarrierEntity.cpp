#include "BarrierEntity.h"

BarrierEntity::BarrierEntity(EntityID id, sf::Vector2f position) : Entity(EntityType::BARRIER, id, position) {}

std::optional<sf::FloatRect> BarrierEntity::GetCollisionBox() const {
	return std::make_optional<sf::FloatRect>(sf::Vector2f(getPosition()), sf::Vector2f(1, 1));
}
