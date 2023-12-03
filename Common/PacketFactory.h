#pragma once
#include <SFML/Network.hpp>
#include "PlayerEntity.h"

const unsigned short DEFAULT_PORT{29011};

using PacketTypeUnderlying = std::uint8_t;
enum class PacketType : PacketTypeUnderlying {
	//Session
	JOIN_GAME,
	//Entity
	ENTITY_CREATE,
	ENTITY_UPDATE,
	ENTITY_DELETE,
	//Player
	PLAYER_SET_CLIENT,
	PLAYER_INPUT,
	PLAYER_SHOOT,
	PLAYER_DAMAGE,
	//Other
	MESSAGE,
	GUN_EFFECTS
};

class PacketFactory {
public:
	static PacketType GetType(sf::Packet& packet);
	// Join Game
	static sf::Packet JoinGame(int tick);
	static int JoinGame(sf::Packet& packet);
	// Delete Entity
	static sf::Packet EntityDelete(EntityID id);
	static EntityID EntityDelete(sf::Packet& packet);
	// Set Client Player
	static sf::Packet PlayerSetClientID(std::optional<EntityID> entityId);
	static std::optional<EntityID> PlayerSetClientID(sf::Packet& packet);
	// Send Player Input
	static sf::Packet PlayerInput(PlayerEntity::InputData inputData);
	static PlayerEntity::InputData PlayerInput(sf::Packet& packet);
	// Shoot
	static sf::Packet PlayerShoot(int tick);
	static int PlayerShoot(sf::Packet& packet);
	// Gun Effects
	static sf::Packet GunEffects();
	// Damage
	struct PlayerDamageData {
		EntityID id;
		int amount;
	};
	static sf::Packet PlayerDamage(EntityID id, int amount);
	static PlayerDamageData PlayerDamage(sf::Packet& packet);
	// Messaging
	static sf::Packet Message(const std::string& message);
	static std::string Message(sf::Packet& packet);
};