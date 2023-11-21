#pragma once
#include "PlayerEntity.h"

class ClientPlayerEntity : public PlayerEntity, public sf::Drawable {
	sf::CircleShape body;
	sf::RectangleShape gun;
	bool localPlayer;
public:
	ClientPlayerEntity(std::uint16_t id, sf::Vector2f position, float rotation, bool localPlayer = false);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	InputData GetInputData(sf::RenderWindow& window);
};