#pragma once
#include <BarrierEntity.h>
#include <SFML/Graphics/Drawable.hpp>
class ClientBarrierEntity : public BarrierEntity {
public:
	ClientBarrierEntity(EntityID id, sf::Vector2f position);
	void Draw(sf::RenderWindow& window, int tick)  const override;
};

