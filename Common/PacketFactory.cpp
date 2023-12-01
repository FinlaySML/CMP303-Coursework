#include "PacketFactory.h"

PacketFactory::PacketType PacketFactory::GetType(sf::Packet& packet) {
    PacketTypeUnderlying type;
    packet >> type;
    return (PacketType)type;
}

sf::Packet PacketFactory::JoinGame(int tick, EntityID playerEntityId, sf::Vector2f position, float rotation, const std::vector<JoinGameData::BarrierData>& barrier) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::JOIN_GAME) << tick << playerEntityId << position.x << position.y << rotation;
    packet << barrier.size();
    for (const auto& data : barrier) {
        packet << data.id;
        packet << data.position.x << data.position.y;
    }
    return packet;
}

PacketFactory::JoinGameData PacketFactory::JoinGame(sf::Packet& packet) {
    JoinGameData data;
    packet >> data.tick >> data.playerEntityId >> data.position.x >> data.position.y >> data.rotation;
    size_t size{};
    packet >> size;
    data.barriers.resize(size);
    for (auto& data : data.barriers) {
        packet >> data.id;
        packet >> data.position.x >> data.position.y;
    }
    return data;
}

sf::Packet PacketFactory::PlayerUpdate(const std::vector<PacketFactory::PlayerUpdateData>& updateData) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::PLAYER_UPDATE) << updateData.size();
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
    packet << static_cast<PacketTypeUnderlying>(PacketType::PLAYER_INPUT) << inputData.w << inputData.a << inputData.s << inputData.d << inputData.target.x << inputData.target.y;
    return packet;
}

PlayerEntity::InputData PacketFactory::PlayerInput(sf::Packet& packet) {
    PlayerEntity::InputData data;
    packet >> data.w >> data.a >> data.s >> data.d >> data.target.x >> data.target.y;
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

sf::Packet PacketFactory::GunEffects(GunEffectData data) {
    sf::Packet packet;
    packet << static_cast<PacketTypeUnderlying>(PacketType::GUN_EFFECTS);
    packet << data.bulletHole.has_value();
    if(data.bulletHole.has_value()) {
        packet << data.bulletHole.value().x;
        packet << data.bulletHole.value().y;
    }
    return packet;
}

PacketFactory::GunEffectData PacketFactory::GunEffects(sf::Packet& packet) {
    GunEffectData data;
    bool hasBulletHole;
    packet >> hasBulletHole;
    if(hasBulletHole) {
        sf::Vector2f pos;
        packet >> pos.x;
        packet >> pos.y;
        data.bulletHole = pos;
    }
    return data;
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
