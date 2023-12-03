#pragma once
#include "BulletHoleEntity.h"
#include <SFML/Graphics/Drawable.hpp>

class ClientBulletHoleEntity : public BulletHoleEntity {
public:
    ClientBulletHoleEntity(EntityID id, sf::Vector2f position, float rotation, int creationTick, int despawnTime, sf::Color color = sf::Color::Black);
    void Draw(sf::RenderWindow& window, int tick) const override;
};