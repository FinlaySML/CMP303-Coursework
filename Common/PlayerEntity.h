#pragma once
#include "Entity.h"
#include <vector>
#include "World.h"

class PlayerEntity : public Entity {
public:
	struct InputData {
		bool w, a, s, d;
		sf::Vector2f target;
	};
	PlayerEntity(EntityID id, sf::Vector2f position, float rotation);
	void Update(sf::Vector2f position, float rotation);
	sf::FloatRect GetCollisionBox() const override;
	void Collision(World* world);
	void Update(float deltaTime, InputData inputData);
	int GetHealth() const;
	void Damage(int amount);
private:
	int health;
};