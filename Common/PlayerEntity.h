#pragma once
#include "Entity.h"
#include <vector>
#include "World.h"

struct PlayerState {
	sf::Vector2f position;
	float rotation;
	int gunCooldown;
};

class PlayerEntity : public Entity {
public:
	struct InputData {
		int index;
		bool w, a, s, d;
		bool leftMouse, rightMouse;
		sf::Vector2f target;
	};
	PlayerEntity(EntityID id, sf::Vector2f position, float rotation);
	virtual void UpdateFromInput(World* world, InputData inputData, bool real = true);
	void Damage(EntityID source, int amount);
	std::optional<sf::FloatRect> GetCollisionBox() const override;
	int GetHealth() const;
	EntityID GetCauseOfDeath() const;
protected:
	int gunCooldown;
	int health;
	EntityID causeOfDeath;
};