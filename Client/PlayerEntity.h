#pragma once

#include <SFML/Graphics.hpp>
class PlayerEntity {
	sf::RectangleShape shape;
public:
	PlayerEntity();
	void Draw(sf::RenderWindow& window);
	void Update(sf::RenderWindow& window);
};