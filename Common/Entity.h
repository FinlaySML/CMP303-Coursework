#pragma once
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network/Packet.hpp>
#include <optional>

class World;

using EntityID = std::uint32_t;
using EntityTypeUnderlying = std::uint8_t;
enum class EntityType : EntityTypeUnderlying {
	UNKNOWN,
	PLAYER,
	BARRIER,
	BULLET_HOLE,
	ROCKET
};

class Entity : public sf::Transformable {
public:
	Entity(EntityType type, EntityID id, sf::Vector2f position = {0,0}, float rotation = 0);
	virtual ~Entity();
	EntityType GetType() const;
	EntityID GetID() const;
	sf::Vector2f getDirection() const;
	//Render
	virtual void Update(World* world);
	virtual void Draw(sf::RenderWindow& window, int tick) const;
	//Networking
	virtual sf::Packet CreationPacket(int tick) const;
	virtual sf::Packet UpdatePacket(int tick) const;
	virtual void UpdateFromPacket(sf::Packet& packet);
	//Physics
	virtual std::optional<sf::FloatRect> GetCollisionBox() const;
	virtual bool ContainsPoint(sf::Vector2f point, int ticksPast = 0) const;
	bool ShouldRemove() const;
	void Kill();
private:
	EntityType type;
	EntityID id;
	bool shouldRemove;
};