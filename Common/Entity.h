#pragma once
#include <SFML/Graphics/Transformable.hpp>

using EntityID = std::uint16_t;

enum class EntityType {
	PLAYER,
	BARRIER
};

class Entity : public sf::Transformable {
public:
	Entity(EntityType type, EntityID id, sf::Vector2f position = {0,0}, float rotation = 0);
	virtual sf::FloatRect GetCollisionBox() const = 0;
	EntityType GetType() const;
	EntityID GetID() const;
	sf::Vector2f getDirection() const;
private:
	EntityType type;
	EntityID id;
};