#pragma once
#include "PlayerEntity.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <unordered_map>
#include "PacketFactory.h"
#include "Interpolator.h"

class ClientWorld;

class ClientPlayerEntity : public PlayerEntity {
	sf::CircleShape body;
	sf::RectangleShape gun;
	bool localPlayer;
	int damageReddening;
	Interpolator interpolator;
public:
	ClientPlayerEntity(EntityID id, int tick, sf::Vector2f position, float rotation);
	void Draw(sf::RenderWindow& window, int tick) const;
	void UpdateFromInput(World* world, InputData inputData) override;
	void UpdateFromPacket(sf::Packet& packet) override;
	void Update(World* world) override;
	void Damage(int amount);
	void SetLocalPlayer(bool value);
};