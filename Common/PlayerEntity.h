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
	virtual void Update(sf::Vector2f position, float rotation);
	void UpdateWithInput(float deltaTime, InputData inputData);
	void Collision(World* world);
	void Damage(int amount);
	sf::FloatRect GetCollisionBox() const override;
	int GetHealth() const;
protected:
	int health;
};