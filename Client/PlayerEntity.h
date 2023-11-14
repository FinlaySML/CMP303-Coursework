#pragma once

#include <SFML/Graphics.hpp>
class PlayerEntity : public sf::Transformable, public sf::Drawable {
	sf::RectangleShape gun;
	sf::CircleShape body;
	sf::RectangleShape sight;
public:
	PlayerEntity();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void Update(float deltaTime, sf::RenderWindow& window);
};