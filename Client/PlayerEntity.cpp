#include "PlayerEntity.h"

PlayerEntity::PlayerEntity() : body{0.4f}, gun{sf::Vector2f{0.3f, 0.2f}} {
	body.setFillColor({ 0,128,0 });
	gun.setFillColor({ 10,10,10 });
	body.setOrigin(body.getRadius(), body.getRadius());
	gun.setOrigin(gun.getSize()*0.5f);
	gun.setPosition(body.getRadius(), 0);
}

void PlayerEntity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(body, states);
	target.draw(gun, states);
}

void PlayerEntity::Update(float deltaTime, sf::RenderWindow& window) {
	sf::Vector2f pos = getPosition();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		pos.x += deltaTime * 5.0f;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		pos.x -= deltaTime * 5.0f;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
		pos.y += deltaTime * 5.0f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		pos.y -= deltaTime * 5.0f;
	}
	setPosition(pos);
	sf::Vector2f diff = window.mapPixelToCoords(sf::Mouse::getPosition(window))-pos;
	setRotation(std::atan2f(diff.y, diff.x)*180.0f/3.1415926535f);
}
	