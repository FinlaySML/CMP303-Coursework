#pragma once
#include "PlayerEntity.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <unordered_map>
#include "PacketFactory.h"

class ClientWorld;

class ClientPlayerEntity : public PlayerEntity {
	sf::CircleShape body;
	sf::RectangleShape gun;
	bool localPlayer;
	int gunCooldown;
	int damageReddening;
public:
	ClientPlayerEntity(EntityID id, sf::Vector2f position, float rotation, bool localPlayer = false);
	void Draw(sf::RenderWindow& window, int tick) const;
	void Update(World* world) override;
	InputData GetInputData(sf::RenderWindow& window);
	struct ShootData {
		bool fired;
		std::optional<sf::Vector2f> bulletHole;
	};
	ShootData UpdateShoot(ClientWorld* world);
	void Damage(int amount);
	void SetLocalPlayer(bool value);
};