#pragma once
#include <SFML/Network.hpp>
#include "PlayerEntity.h"

const std::uint16_t DEFAULT_PORT{29011};

class PacketFactory {
public:
	enum class PacketType : std::uint8_t {
		JOIN_GAME,
		PLAYER_UPDATE,
		PLAYER_INPUT,
		MESSAGE
	};
	static PacketType GetType(sf::Packet& packet);
	// Join Game
	struct JoinGameData {
		std::uint16_t playerEntityId;
		sf::Vector2f position;
		float rotation;
	};
	static sf::Packet JoinGame(std::uint16_t playerEntityId, sf::Vector2f position, float rotation);
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
	// Messaging
	static sf::Packet Message(const std::string& message);
	static std::string Message(sf::Packet& packet);
};