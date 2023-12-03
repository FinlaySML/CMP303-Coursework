#include "World.h"
#include <unordered_set>
#include <stdexcept>
#include <format>

World::World(unsigned short port, int startingTick) : tickClock{60, startingTick}, udp{port} {
}

void World::AddEntity(Entity* entity) {
	entities.insert({entity->GetID(), std::unique_ptr<Entity>(entity)});
}

void World::RemoveEntity(EntityID id) {
    entities.erase(id);
}

Entity* World::GetEntity(EntityID id, EntityType type) {
    auto entity{TryGetEntity(id, type)};
    if(entity) {
        return entity.value();
    }
    throw std::out_of_range(std::format("Could not find entity with id {} and type {}", (int)id, (int)type));
}

std::optional<Entity*> World::TryGetEntity(EntityID id, EntityType type) {
    auto it = entities.find(id);
    if (it == entities.end()) {
        return {};
    }
    if(type != EntityType::UNKNOWN && it->second.get()->GetType() != type ) {
        return {};
    }
    return it->second.get();
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

std::vector<World::RayCastResult> World::RayCast(Entity* exclude, sf::Vector2f origin, sf::Vector2f direction, float maxDistance) {
    std::vector<World::RayCastResult> result;
    std::unordered_set<EntityID> alreadyHit;
    for(float distance = 0.0f; distance < maxDistance; distance += 0.1f){
        sf::Vector2f point{origin + direction * distance};
        for(auto& [id, entity] : entities) {
            if(entity.get() == exclude) continue;
            if(!entity->GetCollisionBox()) continue;
            if(entity->GetCollisionBox().value().contains(point) && !alreadyHit.contains(id)) {
                result.push_back({entity.get(), distance});
                alreadyHit.insert(id);
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
