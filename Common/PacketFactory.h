#pragma once
#include <SFML/Network.hpp>
#include "PlayerEntity.h"
#include "Stats.h"

const unsigned short DEFAULT_PORT{29011};

using PacketTypeUnderlying = std::uint8_t;
enum class PacketType : PacketTypeUnderlying {
	//Session
	NONE,//SERVER
	SET_TICK,//SERVER
	ACK_TICK,//CLIENT
	TELL_RTT,//SERVER
	//MODE
	MODE_RESPAWNING,//SERVER
	MODE_PLAYING,//SERVER
	//Stat
	STAT_UPDATE,//SERVER
	//Entity
	ENTITY_CREATE,//SERVER
	ENTITY_UPDATE,//SERVER
	ENTITY_DELETE,//SERVER
	//Player
	PLAYER_INPUT,//CLIENT
	PLAYER_DAMAGE,//SERVER
	PLAYER_STATE,//SERVER
	//Other
	MESSAGE,//SERVER
	GUN_EFFECTS//SERVER
};

class PacketFactory {
public:
	static PacketType GetType(sf::Packet& packet);
	// None
	static sf::Packet None();
	// Set Tick
	static sf::Packet SetTick(int tick);
	static int SetTick(sf::Packet& packet);
	// Acknowledge Tick
	static sf::Packet AckTick(int tick);
	static int AckTick(sf::Packet& packet);
	// Tell Round-trip-time
	static sf::Packet TellRTT(int rtt);
	static int TellRTT(sf::Packet& packet);
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
	// Damage
	struct PlayerDamageData {
		EntityID id;
		int amount;
	};
	static sf::Packet PlayerDamage(EntityID id, int amount);
	static PlayerDamageData PlayerDamage(sf::Packet& packet);
	// Player state
	struct PlayerStateData {
		sf::Vector2f position;
		float rotation;
		int gunCooldown;
		int rocketCooldown;
		int index;
	};
	static sf::Packet PlayerState(PlayerStateData state);
	static PlayerStateData PlayerState(sf::Packet& packet);
	// Messaging
	static sf::Packet Message(const std::string& message);
	static std::string Message(sf::Packet& packet);
	// Gun sound
	struct GunEffectsData {
		EntityID sourceEntity;
		EntityID hitEntity;
		sf::Vector2f hitPosition;
	};
	static sf::Packet GunEffects(EntityID sourceEntity, EntityID hitEntity, sf::Vector2f hitPosition);
	static GunEffectsData GunEffects(sf::Packet& packet);
};