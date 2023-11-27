#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>
#include "Entity.h"

class World {
public:
	World();
	void AddEntity(Entity* entity);
	void RemoveEntity(EntityID entity);
	std::optional<Entity*> GetEntity(EntityID id);
	struct IntersectionResult {
		Entity* entity;
		float overlap;
	};
	std::vector<IntersectionResult> GetIntersecting(Entity* source);
	struct RayCastResult {
		Entity* entity;
		float distance;
	};
	std::vector<RayCastResult> RayCast(Entity* exclude, sf::Vector2f origin, sf::Vector2f direction, float maxDistance = 100.f);
protected:
	std::unordered_map<EntityID, std::unique_ptr<Entity>> entities;
};

