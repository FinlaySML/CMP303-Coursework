#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>
#include "Entity.h"
#include "TickClock.h"

class World {
public:
	World(int startingTick = 0);
	void AddEntity(Entity* entity);
	void RemoveEntity(EntityID entity);
	Entity* GetEntity(EntityID id, EntityType type = EntityType::UNKNOWN);
	std::optional<Entity*> TryGetEntity(EntityID id, EntityType type = EntityType::UNKNOWN);
	std::vector<Entity*> GetEntities();
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
	virtual void GunEffects(EntityID sourceEntity, EntityID hitEntity, sf::Vector2f hitPosition) = 0;
	const TickClock& GetClock() const;
protected:
	void CleanEntities();
	TickClock tickClock;
	std::unordered_map<EntityID, std::unique_ptr<Entity>> entities;
};

