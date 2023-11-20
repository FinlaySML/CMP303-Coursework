#include "ClientPlayerEntity.h"
#include <iostream>

ClientPlayerEntity::ClientPlayerEntity(std::uint16_t entityId, sf::Vector2f position, float rotation, bool localPlayer) : PlayerEntity(entityId, position, rotation), body{0.4f}, gun{sf::Vector2f{0.3f, 0.2f}}, sight{sf::Vector2f{10.0f, 0.03f}}, localPlayer{localPlayer} {
	body.setFillColor({ 0,128,0 });
	body.setOrigin(body.getRadius(), body.getRadius());
	gun.setFillColor({ 0,64,0 });
	gun.setOrigin(gun.getSize() * 0.5f);
	gun.setPosition(body.getRadius(), 0);
	sight.setFillColor({ 255,255,255 });
	sight.setOrigin(0, sight.getSize().y * 0.5f);
}

void ClientPlayerEntity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	if(localPlayer){
		target.draw(sight, states);
	}
	target.draw(body, states);
	target.draw(gun, states);
}

PlayerEntity::InputData ClientPlayerEntity::GetInputData(sf::RenderWindow& window) {
	InputData inputData{};
	inputData.w = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
	inputData.a = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
	inputData.s = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
	inputData.d = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
	sf::Vector2f diff = window.mapPixelToCoords(sf::Mouse::getPosition(window)) - getPosition();
	sf::Vector2f size = sight.getSize();
	size.x = std::sqrt(diff.x * diff.x + diff.y * diff.y);
	sight.setSize(size);
	inputData.rotation = std::atan2f(diff.y, diff.x) * 180.0f / 3.1415926535f;
	return inputData;
}