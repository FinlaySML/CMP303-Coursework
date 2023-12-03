#include "PacketFactory.h"

PacketType PacketFactory::GetType(sf::Packet& packet) {
    PacketTypeUnderlying type;
    packet >> type;
    return (PacketType)type;
}

sf::Packet PacketFactory::JoinGame(int tick) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::JOIN_GAME);
    packet << tick;
    return packet;
}

int PacketFactory::JoinGame(sf::Packet& packet) {
    int tick;
    packet >> tick;
    return tick;
}

sf::Packet PacketFactory::EntityDelete(EntityID id) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::ENTITY_DELETE);
    packet << id;
    return packet;
}

EntityID PacketFactory::EntityDelete(sf::Packet& packet) {
    EntityID id;
    packet >> id;
    return id;
}

sf::Packet PacketFactory::PlayerSetClientID(std::optional<EntityID> entityId) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::PLAYER_SET_CLIENT);
    bool hasValue{entityId.has_value()};
    packet << hasValue;
    if(hasValue) {
        packet << entityId.value();
    }
    return packet;
}

std::optional<EntityID> PacketFactory::PlayerSetClientID(sf::Packet& packet) {
    bool hasValue;
    packet >> hasValue;
    if(hasValue) {
        EntityID id;
        packet >> id;
        return id;
    }else{
        return {};
    }
}

sf::Packet PacketFactory::PlayerInput(PlayerEntity::InputData inputData) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::PLAYER_INPUT);
    packet << inputData.w << inputData.a << inputData.s << inputData.d;
    packet << inputData.target.x << inputData.target.y;
    return packet;
}

PlayerEntity::InputData PacketFactory::PlayerInput(sf::Packet& packet) {
    PlayerEntity::InputData data;
    packet >> data.w >> data.a >> data.s >> data.d;
    packet >> data.target.x >> data.target.y;
    return data;
}

sf::Packet PacketFactory::PlayerShoot(int tick) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::PLAYER_SHOOT) << tick;
    return packet;
}

int PacketFactory::PlayerShoot(sf::Packet& packet) {
    int tick{};
    packet >> tick;
    return tick;
}

sf::Packet PacketFactory::GunEffects() {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::GUN_EFFECTS);
    return packet;
}

sf::Packet PacketFactory::PlayerDamage(EntityID id, int amount) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::PLAYER_DAMAGE);
    packet << id;
    packet << amount;
    return packet;
}

PacketFactory::PlayerDamageData PacketFactory::PlayerDamage(sf::Packet& packet) {
    PacketFactory::PlayerDamageData data{};
    packet >> data.id;
    packet >> data.amount;
    return data;
}

sf::Packet PacketFactory::Message(const std::string& message) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::MESSAGE) << message;
    return packet;
}

std::string PacketFactory::Message(sf::Packet& packet) {
    std::string message;
    packet >> message;
    return message;
}
