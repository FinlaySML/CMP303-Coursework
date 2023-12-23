#include "PacketFactory.h"

PacketType PacketFactory::GetType(sf::Packet& packet) {
    PacketTypeUnderlying type;
    packet >> type;
    return (PacketType)type;
}

sf::Packet PacketFactory::SetTick(int tick) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::SET_TICK);
    packet << tick;
    return packet;
}

int PacketFactory::SetTick(sf::Packet& packet) {
    int tick;
    packet >> tick;
    return tick;
}

sf::Packet PacketFactory::AckTick(int tick) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::ACK_TICK);
    packet << tick;
    return packet;
}

int PacketFactory::AckTick(sf::Packet& packet) {
    int tick;
    packet >> tick;
    return tick;
}

sf::Packet PacketFactory::TellRTT(int tick) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::TELL_RTT);
    packet << tick;
    return packet;
}

int PacketFactory::TellRTT(sf::Packet& packet) {
    int rtt;
    packet >> rtt;
    return rtt;
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
    packet << inputData.index;
    packet << inputData.w << inputData.a << inputData.s << inputData.d;
    packet << inputData.leftMouse << inputData.rightMouse;
    packet << inputData.target.x << inputData.target.y;
    return packet;
}

PlayerEntity::InputData PacketFactory::PlayerInput(sf::Packet& packet) {
    PlayerEntity::InputData inputData{};
    packet >> inputData.index;
    packet >> inputData.w >> inputData.a >> inputData.s >> inputData.d;
    packet >> inputData.leftMouse >> inputData.rightMouse;
    packet >> inputData.target.x >> inputData.target.y;
    return inputData;
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

sf::Packet PacketFactory::PlayerState(PlayerStateData state) {
    sf::Packet packet{};
    packet << static_cast<PacketTypeUnderlying>(PacketType::PLAYER_STATE);
    packet << state.position.x << state.position.y;
    packet << state.rotation;
    packet << state.gunCooldown;
    packet << state.index;
    return packet;
}

PacketFactory::PlayerStateData PacketFactory::PlayerState(sf::Packet& packet) {
    PlayerStateData data;
    packet >> data.position.x >> data.position.y;
    packet >> data.rotation;
    packet >> data.gunCooldown;
    packet >> data.index;
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


sf::Packet PacketFactory::GunEffects(EntityID sourceEntity, EntityID hitEntity, sf::Vector2f hitPosition) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::GUN_EFFECTS);
    packet << sourceEntity;
    packet << hitEntity;
    packet << hitPosition.x;
    packet << hitPosition.y;
    return packet;
}

PacketFactory::GunEffectsData PacketFactory::GunEffects(sf::Packet& packet) {
    PacketFactory::GunEffectsData data;
    packet >> data.sourceEntity;
    packet >> data.hitEntity;
    packet >> data.hitPosition.x;
    packet >> data.hitPosition.y;
    return data;
}