#include "ClientPlayerEntity.h"
#include "ClientWorld.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <random>

ClientPlayerEntity::ClientPlayerEntity(EntityID entityId, sf::Vector2f position, float rotation, bool localPlayer) : 
PlayerEntity(entityId, position, rotation), 
body{0.4f}, 
gun{sf::Vector2f{0.3f, 0.2f}}, 
localPlayer{localPlayer}, 
gunCooldown{0},
damageReddening{0} {
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

void ClientPlayerEntity::Update(sf::Vector2f position, float rotation) {
	PlayerEntity::Update(position, rotation);
	if (damageReddening > 0 ) {
		damageReddening--;
		body.setFillColor({ 255,55,0 });
	} else {
		body.setFillColor({ 0,128,0 });
	}
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

std::optional<PacketFactory::GunEffectData> ClientPlayerEntity::UpdateShoot(ClientWorld* world) {
	if(gunCooldown > 0) gunCooldown--;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && gunCooldown == 0) {
		PacketFactory::GunEffectData data;
		gunCooldown = 5;
		auto result = world->RayCast(this, getPosition(), getDirection());
		if(result.size() > 0) {
			if(result[0].entity->GetType() == EntityType::BARRIER) {
				data.bulletHole = getPosition() + getDirection() * result[0].distance;
			}
		}
		return data;
	}
	return {};
}

void ClientPlayerEntity::Damage(int amount) {
	PlayerEntity::Damage(amount);
	damageReddening = 4;
}
