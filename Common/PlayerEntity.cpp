#include "PlayerEntity.h"
#include <iostream>
#include <array>

PlayerEntity::PlayerEntity(std::uint16_t entityId, sf::Vector2f position, float rotation) : id{ entityId } {
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

void PlayerEntity::Collision(const std::vector<sf::Vector2i>& barriers) {
    for (int i = 0; i < 2; i++) {
        float overlap = 0.0f;
        size_t mostOverlap = 0;
        for (size_t barrierIndex = 0; barrierIndex < barriers.size(); barrierIndex++) {
            sf::FloatRect barrier{ sf::Vector2f(barriers[barrierIndex]), sf::Vector2f(1,1) };
            sf::FloatRect body{ GetCollisionBox() };
            if (barrier.intersects(body)) {
                float top = std::max(barrier.top, body.top);
                float left = std::max(barrier.left, body.left);
                float bottom = std::min(barrier.top + barrier.height, body.top + body.height);
                float right = std::min(barrier.left + barrier.width, body.left + body.width);
                float thisOverlap = (bottom - top) * (right - left);
                if (thisOverlap > overlap) {
                    overlap = thisOverlap;
                    mostOverlap = barrierIndex;
                }
            }
        }
        if (overlap == 0.0f) {
            break;
        }
        sf::FloatRect barrier{ sf::Vector2f(barriers[mostOverlap]), sf::Vector2f(1,1) };
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

sf::FloatRect PlayerEntity::GetCollisionBox() const
{
	return { getPosition() - sf::Vector2f(0.4f, 0.4f), sf::Vector2f(0.8f, 0.8f) };
}
