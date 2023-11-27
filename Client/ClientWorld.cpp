#include "ClientWorld.h"
#include <SFML/Graphics/Sprite.hpp>
#include "ClientPlayerEntity.h"

ClientWorld::ClientWorld() {
    brickTexture.loadFromFile("brick.png");
}

void ClientWorld::Render(sf::RenderWindow& window) {
    sf::Sprite brick{ brickTexture };
    brick.setScale(1.0f / 16.0f, 1.0f / 16.0f);
    for (auto& [id, entity] : entities) {
        if(entity->GetType() == EntityType::BARRIER) {
            brick.setPosition(entity->getPosition());
            window.draw(brick);
        }
    }
    for (auto& [id, entity] : entities) {
        if (entity->GetType() == EntityType::PLAYER) {
            window.draw(*(ClientPlayerEntity*)entity.get());
        }
    }
}
