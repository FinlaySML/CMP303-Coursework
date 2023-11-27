#include "Entity.h"

Entity::Entity(EntityType type, EntityID id, sf::Vector2f position, float rotation) : type{type}, id{id} {
	setPosition(position);
	setRotation(rotation);
}

EntityType Entity::GetType() const {
	return type;
}

EntityID Entity::GetID() const {
	return id;
}

sf::Vector2f Entity::getDirection() const {
	float radians{ getRotation() / 180.0f * 3.1415926f };
	return sf::Vector2f{ cosf(radians), sinf(radians) };
}
