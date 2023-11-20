#include "PlayerEntity.h"
#include <iostream>

PlayerEntity::PlayerEntity(std::uint16_t entityId, sf::Vector2f position, float rotation) : id{ entityId } {
	Update(position, rotation);
}

void PlayerEntity::Update(sf::Vector2f position, float rotation) {
	setPosition(position);
	setRotation(rotation);
}

void PlayerEntity::Update(float deltaTime, InputData inputData) {
	sf::Vector2f pos = getPosition();
	if (inputData.d) {
		pos.x += deltaTime * 5.0f;
	}
	if (inputData.a) {
		pos.x -= deltaTime * 5.0f;
	}
	if (inputData.s) {
		pos.y += deltaTime * 5.0f;
	}
	if (inputData.w) {
		pos.y -= deltaTime * 5.0f;
	}
	Update(pos, inputData.rotation);
}

sf::FloatRect PlayerEntity::GetCollisionBox() const
{
	return { getPosition() - sf::Vector2f(0.4f, 0.4f), sf::Vector2f(0.8f, 0.8f) };
}
