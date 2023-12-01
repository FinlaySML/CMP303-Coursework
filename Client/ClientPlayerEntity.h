#pragma once
#include "PlayerEntity.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <unordered_map>
#include "PacketFactory.h"

class ClientWorld;

class ClientPlayerEntity : public PlayerEntity, public sf::Drawable {
	sf::CircleShape body;
	sf::RectangleShape gun;
	bool localPlayer;
	int gunCooldown;
	int damageReddening;
public:
	ClientPlayerEntity(EntityID id, sf::Vector2f position, float rotation, bool localPlayer = false);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void Update(sf::Vector2f position, float rotation) override;
	InputData GetInputData(sf::RenderWindow& window);
	std::optional<PacketFactory::GunEffectData> UpdateShoot(ClientWorld* world);
	void Damage(int amount);
};