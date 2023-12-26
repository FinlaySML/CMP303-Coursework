#include "ClientRocketEntity.h"
#include <SFML/Graphics/RectangleShape.hpp>

ClientRocketEntity::ClientRocketEntity(EntityID id, sf::Vector2f position, float rotation, EntityID sourceEntity, int creationTick, int despawnTime) :
RocketEntity(id, position, rotation, sourceEntity, creationTick, despawnTime) {
}

void ClientRocketEntity::Draw(sf::RenderWindow& window, int tick) const {
    sf::RectangleShape rocket{sf::Vector2f{ 0.70f, 0.15f }};
    rocket.setOrigin(rocket.getSize() * 0.5f);
    window.draw(rocket, getTransform());
}
