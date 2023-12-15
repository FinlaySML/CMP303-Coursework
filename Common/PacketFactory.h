#pragma once
#include <SFML/Network.hpp>
#include "PlayerEntity.h"
#include "Stats.h"

const unsigned short DEFAULT_PORT{29011};

using PacketTypeUnderlying = std::uint8_t;
enum class PacketType : PacketTypeUnderlying {
	//Session
	NONE,
	PING,
	PONG,
	//MODE
	MODE_RESPAWNING,
	MODE_PLAYING,
	//Stat
	STAT_UPDATE,
	//Entity
	ENTITY_CREATE,
	ENTITY_UPDATE,
	ENTITY_DELETE,
	//Player
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
	// None
	static sf::Packet None();
	// Ping Pong
	static sf::Packet Ping();
	static sf::Packet Pong(int tick);
	static int Pong(sf::Packet& packet);
	// Mode Respawning
	static sf::Packet ModeRespawning(float respawnTime);
	static float ModeRespawning(sf::Packet& packet);
	// Mode Playing
	static sf::Packet ModePlaying(EntityID playerID);
	static EntityID ModePlaying(sf::Packet& packet);
	// Stat Update
	struct StatUpdateData {
		Stats::Type type;
		int value;
	};
	static sf::Packet StatUpdate(Stats::Type type, int value);
	static StatUpdateData StatUpdate(sf::Packet& packet);
	// Delete Entity
	static sf::Packet EntityDelete(EntityID id);
	static EntityID EntityDelete(sf::Packet& packet);
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