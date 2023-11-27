#include "PlayerEntity.h"
#include <iostream>
#include <array>
#include <ranges>

PlayerEntity::PlayerEntity(EntityID id, sf::Vector2f position, float rotation) : Entity(EntityType::PLAYER, id), health{1000} {
	Update(position, rotation);
}

void PlayerEntity::Update(sf::Vector2f position, float rotation) {
	setPosition(position);
	setRotation(rotation);
}

std::array<sf::Vector2f, 4> directions = {
    sf::Vector2f(-1, 0),
    sf::Vector2f(1, 0),
    sf::Vector2f(0,-1),
    sf::Vector2f(0, 1)
};

void PlayerEntity::Collision(World* world) {
    for (int i = 0; i < 2; i++) {
        auto result = world->GetIntersecting(this);
        if (result.empty()) {
            break;
        }
        auto maxOverlap = std::ranges::max_element(result, {}, [](World::IntersectionResult& ir){return ir.overlap;});
        sf::FloatRect barrier{ maxOverlap->entity->GetCollisionBox() };
        sf::FloatRect body{ GetCollisionBox() };
        std::array<float, 4> ejects = {
            std::abs(barrier.left - body.width - body.left), //left
            std::abs(barrier.left + barrier.width - body.left), //right
            std::abs(barrier.top - body.height - body.top), //top
            std::abs(barrier.top + barrier.height - body.top) //bottom
        };
        size_t index = std::distance(ejects.begin(), std::min_element(ejects.begin(), ejects.end()));
        move(directions[index] * ejects[index]);
    }
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
    sf::Vector2f diff = inputData.target - getPosition();
    float rotation = std::atan2f(diff.y, diff.x) * 180.0f / 3.1415926535f;
	Update(pos, rotation);
}

int PlayerEntity::GetHealth() const {
    return health;
}

void PlayerEntity::Damage(int amount) {
    health -= amount;
    if(health < 0) {
        health = 0;
    }
}

sf::FloatRect PlayerEntity::GetCollisionBox() const
{
	return { getPosition() - sf::Vector2f(0.4f, 0.4f), sf::Vector2f(0.8f, 0.8f) };
}
