#pragma once
#include "PlayerEntity.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <unordered_map>
#include "ClientWorld.h"

class ClientPlayerEntity : public PlayerEntity, public sf::Drawable {
	sf::CircleShape body;
	sf::RectangleShape gun;
	bool localPlayer;
	int gunCooldown;
public:
	ClientPlayerEntity(std::uint16_t id, sf::Vector2f position, float rotation, bool localPlayer = false);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	InputData GetInputData(sf::RenderWindow& window);
	struct ShootData {
		bool firedGun;
		std::optional<sf::Vector2f> bulletHole;
	};
	ShootData UpdateShoot(ClientWorld* world);
};