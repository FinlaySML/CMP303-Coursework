#pragma once
#include <World.h>
#include "ConnectedSocket.h"
#include "ClientPlayerEntity.h"
#include "PacketFactory.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio.hpp>

class ClientWorld : public World {
public:
	ClientWorld(std::unique_ptr<sf::TcpSocket>&& server, int initialTick);
    void Update(sf::RenderWindow& window);
    void Render(sf::RenderWindow& window);
    Entity* CreateFromPacket(sf::Packet& packet);
    bool Disconnected() const;
    std::optional<EntityID> GetLocalPlayer() const;
private:
    //Network
    ConnectedSocket server;
    //Input
    sf::Vector2f lastMousePosition;
    //Other
    std::optional<EntityID> localPlayer;
};

