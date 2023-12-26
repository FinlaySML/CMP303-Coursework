#include "World.h"
#include <unordered_set>
#include <stdexcept>
#include <format>
#include <ranges>

World::World(Side side, int startingTick) : 
side{side}, 
tickClock{60, startingTick} {
}

World::Side World::GetSide() const {
    return side;
}

void World::AddEntity(Entity* entity) {
	entities.insert({entity->GetID(), std::unique_ptr<Entity>(entity)});
}

void World::RemoveEntity(EntityID id) {
    entities.erase(id);
}

Entity* World::GetEntity(EntityID id, EntityType type) const {
    auto entity{TryGetEntity(id, type)};
    if(entity) {
        return entity.value();
    }
    throw std::out_of_range(std::format("Could not find entity with id {} and type {}", (int)id, (int)type));
}

std::optional<Entity*> World::TryGetEntity(EntityID id, EntityType type) const {
    auto it = entities.find(id);
    if (it == entities.end()) {
        return {};
    }
    if(type != EntityType::UNKNOWN && it->second.get()->GetType() != type ) {
        return {};
    }
    return it->second.get();
}

std::vector<Entity*> World::GetEntities() {
    std::vector<Entity*> result;
    for(auto& [id, entity] : entities) {
        result.push_back(entity.get());
    }
    return result;
}

std::vector<World::IntersectionResult> World::GetIntersecting(Entity* source) {
    auto sourceOptional{ source->GetCollisionBox() };
    if (!sourceOptional) {
        return {};
    }
    std::vector<World::IntersectionResult> result;
    for (auto& [id, entity] : entities) {
        if(entity->GetType() == EntityType::BARRIER) {
            auto barrierOptional{ entity->GetCollisionBox() };
            if(barrierOptional) {
                sf::FloatRect& barrier{ barrierOptional.value() };
                sf::FloatRect& body{ sourceOptional.value() };
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
    }
	return result;
}

std::vector<World::RayCastResult> World::RayCast(EntityID exclude, sf::Vector2f origin, sf::Vector2f direction, float maxDistance, float stepSize, int ticksPast) {
    std::vector<World::RayCastResult> result;
    EntityID alreadyHit{0};
    for(float distance = 0.0f; distance < maxDistance; distance += 0.25f){
        sf::Vector2f point{origin + direction * distance};
        for(auto& [id, entity] : entities) {
            if(id == exclude) continue;
            if(id == alreadyHit) continue;
            if(entity->ContainsPoint(point, ticksPast)) {
                result.push_back({entity.get(), distance});
                alreadyHit = id;
            }
        }
    }
	return result;
}

const TickClock& World::GetClock() const {
    return tickClock;
}

void World::CleanEntities() {
    std::erase_if(entities, [](const auto& pair) {
        auto const& [id, entity] = pair;
        return entity->ShouldRemove();
    });
}
