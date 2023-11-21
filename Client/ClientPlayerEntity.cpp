#include "ClientPlayerEntity.h"
#include <iostream>

ClientPlayerEntity::ClientPlayerEntity(std::uint16_t entityId, sf::Vector2f position, float rotation, bool localPlayer) : PlayerEntity(entityId, position, rotation), body{0.4f}, gun{sf::Vector2f{0.3f, 0.2f}}, localPlayer{localPlayer} {
	if (localPlayer) {
		body.setOutlineThickness(0.05f);
		body.setRadius(body.getRadius() - 0.05f);
	}
	body.setFillColor({ 0,128,0 });
	body.setOrigin(body.getRadius(), body.getRadius());
	gun.setFillColor({ 0,64,0 });
	gun.setOrigin(gun.getSize() * 0.5f);
	gun.setPosition(body.getRadius(), 0);
}

void ClientPlayerEntity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(body, states);
	target.draw(gun, states);
}

PlayerEntity::InputData ClientPlayerEntity::GetInputData(sf::RenderWindow& window) {
	InputData inputData{};
	inputData.w = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
	inputData.a = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
	inputData.s = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
	inputData.d = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
	inputData.target = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	return inputData;
}