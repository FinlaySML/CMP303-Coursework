#include "ClientBarrierEntity.h"
#include "ResourceManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

ClientBarrierEntity::ClientBarrierEntity(EntityID id, sf::Vector2f position) : BarrierEntity(id, position) {}

void ClientBarrierEntity::Draw(sf::RenderWindow& window, int tick) const {
    sf::Sprite brick{ ResourceManager::GetInstance().brickTexture };
    brick.setScale(1.0f / 16.0f, 1.0f / 16.0f);
    window.draw(brick, getTransform());
}
