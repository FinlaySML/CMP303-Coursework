#pragma once
#include "Entity.h"

class RocketEntity : public Entity {
protected:
	EntityID sourceEntity;
	int creationTick;
	int despawnTime;
public:
	RocketEntity(EntityID id, sf::Vector2f position, float rotation, EntityID sourceEntity, int creationTick, int despawnTime);
	sf::Packet CreationPacket(int tick) const override;
	void Update(World* world) override;
};

