#include "PacketFactory.h"

PacketFactory::PacketType PacketFactory::GetType(sf::Packet& packet) {
    PacketFactory::PacketType type;
    packet >> (std::uint8_t&)type;
    return type;
}

sf::Packet PacketFactory::JoinGame(std::uint16_t entityId, sf::Vector2f position, float rotation) {
    sf::Packet packet;
    packet << static_cast<std::uint8_t>(PacketType::JOIN_GAME) << entityId << position.x << position.y << rotation;
    return packet;
}

PacketFactory::JoinGameData PacketFactory::JoinGame(sf::Packet& packet) {
    JoinGameData data;
    packet >> data.playerEntityId >> data.position.x >> data.position.y;
    return data;
}

sf::Packet PacketFactory::PlayerUpdate(const std::vector<PacketFactory::PlayerUpdateData>& updateData) {
    sf::Packet packet;
    packet << static_cast<std::uint8_t>(PacketType::PLAYER_UPDATE) << updateData.size();
    for(const auto& data : updateData) {
        packet << data.entityId << data.position.x << data.position.y << data.rotation;
    }
    return packet;
}

std::vector<PacketFactory::PlayerUpdateData> PacketFactory::PlayerUpdate(sf::Packet& packet) {
    std::vector<PlayerUpdateData> updateData;
    size_t updates{};
    packet >> updates;
    updateData.resize(updates);
    for(auto& data : updateData) {
        packet >> data.entityId >> data.position.x >> data.position.y >> data.rotation;
    }
    return updateData;
}

sf::Packet PacketFactory::PlayerInput(PlayerEntity::InputData inputData) {
    sf::Packet packet;
    packet << static_cast<std::uint8_t>(PacketType::PLAYER_INPUT) << inputData.w << inputData.a << inputData.s << inputData.d << inputData.rotation;
    return packet;
}

PlayerEntity::InputData PacketFactory::PlayerInput(sf::Packet& packet) {
    PlayerEntity::InputData data;
    packet >> data.w >> data.a >> data.s >> data.d >> data.rotation;
    return data;
}

sf::Packet PacketFactory::Message(const std::string& message) {
    sf::Packet packet;
    packet << static_cast<std::uint8_t>(PacketType::MESSAGE) << message;
    return packet;
}

std::string PacketFactory::Message(sf::Packet& packet) {
    std::string message;
    packet >> message;
    return message;
}
