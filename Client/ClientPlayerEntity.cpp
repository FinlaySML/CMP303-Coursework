#include "ClientPlayerEntity.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <random>

ClientPlayerEntity::ClientPlayerEntity(std::uint16_t entityId, sf::Vector2f position, float rotation, bool localPlayer) : 
PlayerEntity(entityId, position, rotation), 
body{0.4f}, 
gun{sf::Vector2f{0.3f, 0.2f}}, 
localPlayer{localPlayer}, 
hit{sf::Vector2f{0.2f,0.2f}},
gunCooldown{0} {
	if (localPlayer) {
		body.setOutlineThickness(0.05f);
		body.setRadius(body.getRadius() - 0.05f);
	}
	body.setFillColor({ 0,128,0 });
	body.setOrigin(body.getRadius(), body.getRadius());
	gun.setFillColor({ 0,64,0 });
	gun.setOrigin(gun.getSize() * 0.5f);
	gun.setPosition(body.getRadius(), 0);
	hit.setOrigin(hit.getSize() * 0.5f);
	hit.setPosition(1000.0f, 1000.0f);
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

std::mt19937 randGen;
std::uniform_real_distribution<float> bulletSpread{-2, 2};

ClientPlayerEntity::ShootData ClientPlayerEntity::UpdateShoot(ClientWorld* world) {
	ShootData data{false, {}};
	if(gunCooldown > 0) gunCooldown--;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && gunCooldown == 0) {
		gunCooldown = 10;
		auto result = world->RayCast(this, getPosition(), getDirection());
		if(result.size() > 0) {
			data.bulletHole = getPosition() + getDirection() * result[0].distance;
		}
		data.firedGun = true;
	}
	return data;
}
