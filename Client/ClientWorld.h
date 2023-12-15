#pragma once
#include <World.h>
#include "ClientPlayerEntity.h"
#include "PacketFactory.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio.hpp>
#include "ClientNetworking.h"

class ClientWorld : public World {
public:
	ClientWorld(std::unique_ptr<ClientNetworking>&& server);
    void Update(sf::RenderWindow& window);
    void Render(sf::RenderWindow& window);
    static Entity* CreateFromPacket(sf::Packet& packet);
    bool Disconnected() const;
    std::optional<EntityID> GetLocalPlayer() const;
private:
    std::unique_ptr<ClientNetworking> server;
    sf::Vector2f lastMousePosition;
    std::optional<EntityID> localPlayer;
    float respawnTime;
};

