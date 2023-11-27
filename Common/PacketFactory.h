#pragma once
#include <SFML/Network.hpp>
#include "PlayerEntity.h"

const std::uint16_t DEFAULT_PORT{29011};

class PacketFactory {
public:
	using PacketTypeUnderlying = std::uint8_t;
	enum class PacketType : PacketTypeUnderlying {
		JOIN_GAME,
		PLAYER_UPDATE,
		PLAYER_INPUT,
		PLAYER_SHOOT,
		PLAYER_DAMAGE,
		MESSAGE
	};
	static PacketType GetType(sf::Packet& packet);
	// Join Game
	struct JoinGameData {
		std::uint16_t playerEntityId;
		sf::Vector2f position;
		float rotation;
		struct BarrierData {
			EntityID id;
			sf::Vector2f position;
		};
		std::vector<BarrierData> barriers;
	};
	static sf::Packet JoinGame(std::uint16_t playerEntityId, sf::Vector2f position, float rotation, const std::vector<JoinGameData::BarrierData>& barriers);
	static JoinGameData JoinGame(sf::Packet& packet);
	// Update Player
	struct PlayerUpdateData {
		std::uint16_t entityId;
		sf::Vector2f position;
		float rotation;
	};
	static sf::Packet PlayerUpdate(const std::vector<PlayerUpdateData>& updateData);
	static std::vector<PlayerUpdateData> PlayerUpdate(sf::Packet& packet);
	// Send Player Input
	static sf::Packet PlayerInput(PlayerEntity::InputData inputData);
	static PlayerEntity::InputData PlayerInput(sf::Packet& packet);
	// Shoot
	static sf::Packet PlayerShoot();
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