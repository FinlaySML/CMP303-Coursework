#include "PlayerEntity.h"
#include <iostream>
#include <array>
#include <ranges>

PlayerEntity::PlayerEntity(EntityID id, sf::Vector2f position, float rotation) : 
Entity(EntityType::PLAYER, id), 
health{1000}, 
causeOfDeath{0},
gunCooldown{0} {
	setPosition(position);
    setRotation(rotation);
}

std::array<sf::Vector2f, 4> directions = {
    sf::Vector2f(-1, 0),
    sf::Vector2f( 1, 0),
    sf::Vector2f( 0,-1),
    sf::Vector2f( 0, 1)
};

void PlayerEntity::UpdateFromInput(World* world, InputData inputData, bool real) {
	//Position
    float deltaTime{world->GetClock().GetTickDelta()};
    sf::Vector2f velocity{ float(inputData.d - inputData.a), float(inputData.s - inputData.w) };
	velocity *= 5.0f;
    move(velocity * deltaTime);
    //Rotation
    sf::Vector2f diff = inputData.target - getPosition();
    float rotation = std::atan2f(diff.y, diff.x) * 180.0f / 3.1415926535f;
    setRotation(rotation);
    //Collision
    for (int i = 0; i < 2; i++) {
        auto result = world->GetIntersecting(this);
        if (result.empty()) {
            break;
        }
        auto maxOverlap = std::ranges::max_element(result, {}, [](World::IntersectionResult& ir) {return ir.overlap; });
        sf::FloatRect barrier{ maxOverlap->entity->GetCollisionBox().value() };
        sf::FloatRect body{ GetCollisionBox().value() };
        std::array<float, 4> ejects = {
            std::abs(barrier.left - body.width - body.left), //left
            std::abs(barrier.left + barrier.width - body.left), //right
            std::abs(barrier.top - body.height - body.top), //top
            std::abs(barrier.top + barrier.height - body.top) //bottom
        };
        size_t index = std::distance(ejects.begin(), std::min_element(ejects.begin(), ejects.end()));
        move(directions[index] * ejects[index]);
    }
    //Shoot
    if (gunCooldown > 0) {
        gunCooldown--;
    } else if (inputData.leftMouse) {
        gunCooldown = 5;
        if(real) {
            auto result = world->RayCast(this, getPosition(), getDirection());
            if (result.size() > 0) {
                auto type{ result[0].entity->GetType() };
                world->GunEffects(this->GetID(), result[0].entity->GetID(), getPosition() + getDirection() * result[0].distance);
            }
        }
    }
}

int PlayerEntity::GetHealth() const {
    return health;
}

EntityID PlayerEntity::GetCauseOfDeath() const {
    return causeOfDeath;
}

void PlayerEntity::Damage(EntityID source, int amount) {
    if(health > 0) {
        health -= amount;
        if(health <= 0) {
            causeOfDeath = source;
            health = 0;
        }
    }
}

std::optional<sf::FloatRect> PlayerEntity::GetCollisionBox() const
{
	return std::make_optional<sf::FloatRect>( getPosition() - sf::Vector2f(0.4f, 0.4f), sf::Vector2f(0.8f, 0.8f) );
}
