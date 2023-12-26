#pragma once
#include "RocketEntity.h"

class ClientRocketEntity : public RocketEntity {
public:
	ClientRocketEntity(EntityID id, sf::Vector2f position, float rotation, EntityID sourceEntity, int creationTick, int despawnTime);
	void Draw(sf::RenderWindow& window, int tick) const override;
};

