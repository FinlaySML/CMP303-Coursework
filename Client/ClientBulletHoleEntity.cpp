#include "ClientBulletHoleEntity.h"
#include "ResourceManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

ClientBulletHoleEntity::ClientBulletHoleEntity(EntityID id, sf::Vector2f position, float rotation, int creationTick, int despawnTime, sf::Color color) :
BulletHoleEntity(id, position, rotation, creationTick, despawnTime, color) {
}

void ClientBulletHoleEntity::Draw(sf::RenderWindow& window, int tick) const {
    sf::Sprite bhSprite;
    bhSprite.setTexture(ResourceManager::GetInstance().bulletHoleDecal);
    bhSprite.setScale(1 / 32.0f, 1 / 32.0f);
    bhSprite.setOrigin(bhSprite.getLocalBounds().getSize() / 2.0f);
    sf::Color c{ color };
    c.a *= 1.0f - (tick - creationTick) / (float)despawnTime;
    bhSprite.setColor(c);
    window.draw(bhSprite, getTransform());
}
