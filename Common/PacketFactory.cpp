#include "PacketFactory.h"

PacketType PacketFactory::GetType(sf::Packet& packet) {
    PacketTypeUnderlying type;
    packet >> type;
    return (PacketType)type;
}

sf::Packet PacketFactory::Ping() {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::PING);
    return packet;
}

sf::Packet PacketFactory::Pong(int tick) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::PONG);
    packet << tick;
    return packet;
}

int PacketFactory::Pong(sf::Packet& packet) {
    int data;
    packet >> data;
    return data;
}

sf::Packet PacketFactory::ModeRespawning(float respawnTime) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::MODE_RESPAWNING);
    packet << respawnTime;
    return packet;
}

float PacketFactory::ModeRespawning(sf::Packet& packet) {
    float respawnTime;
    packet >> respawnTime;
    return respawnTime;
}

sf::Packet PacketFactory::ModePlaying(EntityID playerID) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::MODE_PLAYING);
    packet << playerID;
    return packet;
}

EntityID PacketFactory::ModePlaying(sf::Packet& packet) {
    EntityID playerId;
    packet >> playerId;
    return playerId;
}


sf::Packet PacketFactory::StatUpdate(Stats::Type type, int value) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::STAT_UPDATE);
    packet << (Stats::TypeUnderlying)type;
    packet << value;
    return packet;
}

PacketFactory::StatUpdateData PacketFactory::StatUpdate(sf::Packet& packet) {
    StatUpdateData data{};
    packet >> (Stats::TypeUnderlying&)data.type;
    packet >> data.value;
    return data;
}

sf::Packet PacketFactory::None() {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::NONE);
    return packet;
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
