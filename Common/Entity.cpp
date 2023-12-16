#include "Entity.h"
#include "PacketFactory.h"
#include "World.h"

Entity::Entity(EntityType type, EntityID id, sf::Vector2f position, float rotation) : type{type}, id{id}, shouldRemove{false} {
	setPosition(position);
	setRotation(rotation);
}

Entity::~Entity() {
}

EntityType Entity::GetType() const {
	return type;
}

EntityID Entity::GetID() const {
	return id;
}

sf::Vector2f Entity::getDirection() const {
	float radians{ getRotation() / 180.0f * 3.1415926f };
	return sf::Vector2f{ cosf(radians), sinf(radians) };
}

bool Entity::ShouldRemove() const {
	return shouldRemove;
}

void Entity::Kill() {
	shouldRemove = true;
}

void Entity::Update(World* world) {
}

void Entity::Draw(sf::RenderWindow& window, int tick) const {}

sf::Packet Entity::CreationPacket(int tick) const {
	sf::Packet packet;
	packet << static_cast<PacketTypeUnderlying>(PacketType::ENTITY_CREATE);
	packet << static_cast<EntityTypeUnderlying>(type);
	packet << id;
	packet << tick;
	packet << getPosition().x << getPosition().y;
	packet << getRotation();
	return packet;
}

sf::Packet Entity::UpdatePacket(int tick) const {
	sf::Packet packet;
	packet << static_cast<PacketTypeUnderlying>(PacketType::ENTITY_UPDATE);
	packet << id;
	packet << tick;
	packet << getPosition().x << getPosition().y;
	packet << getRotation();
	return packet;
}

void Entity::UpdateFromPacket(sf::Packet& packet) {
	int tick;
	float x, y, r;
	packet >> tick;
	packet >> x >> y;
	packet >> r;
	setPosition({x, y});
	setRotation(r);
}

std::optional<sf::FloatRect> Entity::GetCollisionBox() const {
	return {};
}
