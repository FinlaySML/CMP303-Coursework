#include "PlayerEntity.h"

PlayerEntity::PlayerEntity() : shape{sf::Vector2f{10, 20}} {}

void PlayerEntity::Draw(sf::RenderWindow& window) {
	window.draw(shape);
}

void PlayerEntity::Update(sf::RenderWindow& window) {
	auto pos = shape.getPosition();
	pos.x += sf::Keyboard::isKeyPressed(sf::Keyboard::A);
	pos.x -= sf::Keyboard::isKeyPressed(sf::Keyboard::D);

	pos.y += sf::Keyboard::isKeyPressed(sf::Keyboard::W);
	pos.y -= sf::Keyboard::isKeyPressed(sf::Keyboard::S);
}
	