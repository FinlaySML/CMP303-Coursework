#pragma once
#include "Entity.h"
#include <SFML/Graphics/Color.hpp>

class BulletHoleEntity : public Entity {
protected:
	int creationTick;
	int despawnTime;
	sf::Color color;
public:
	BulletHoleEntity(EntityID id, sf::Vector2f position, float rotation, int creationTick, int despawnTime, sf::Color color = sf::Color::Black);
	sf::Packet CreationPacket(int tick) const override;
	void Update(World* world) override;
};

