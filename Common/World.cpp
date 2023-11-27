#include "World.h"
#include <unordered_set>

World::World() {}

void World::AddEntity(Entity* entity) {
	entities.insert({entity->GetID(), std::unique_ptr<Entity>(entity)});
}

void World::RemoveEntity(EntityID id) {
    entities.erase(id);
}

std::optional<Entity*> World::GetEntity(EntityID id) {
	auto it = entities.find(id);
	if(it == entities.end()) {
		return {};
	}
	return it->second.get();
}

std::vector<World::IntersectionResult> World::GetIntersecting(Entity* source) {
    std::vector<World::IntersectionResult> result;
    for (auto& [id, entity] : entities) {
        if(entity->GetType() == EntityType::BARRIER) {
            sf::FloatRect barrier{ entity->GetCollisionBox()};
            sf::FloatRect body{ source->GetCollisionBox() };
            if (barrier.intersects(body)) {
                float top = std::max(barrier.top, body.top);
                float left = std::max(barrier.left, body.left);
                float bottom = std::min(barrier.top + barrier.height, body.top + body.height);
                float right = std::min(barrier.left + barrier.width, body.left + body.width);
                float thisOverlap = (bottom - top) * (right - left);
                result.push_back({ entity.get(), thisOverlap });
            }
        }
    }
	return result;
}

std::vector<World::RayCastResult> World::RayCast(Entity* exclude, sf::Vector2f origin, sf::Vector2f direction, float maxDistance) {
    std::vector<World::RayCastResult> result;
    std::unordered_set<EntityID> alreadyHit;
    for(float distance = 0.0f; distance < maxDistance; distance += 0.1f){
        sf::Vector2f point{origin + direction * distance};
        for(auto& [id, entity] : entities) {
            if(entity.get() != exclude && entity->GetCollisionBox().contains(point) && !alreadyHit.contains(id)) {
                result.push_back({entity.get(), distance});
                alreadyHit.insert(id);
            }
        }
    }
	return result;
}
