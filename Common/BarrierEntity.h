#pragma once
#include "Entity.h"

class BarrierEntity : public Entity {
public:
	BarrierEntity(EntityID id, sf::Vector2f position);
	sf::FloatRect GetCollisionBox() const override;
};

