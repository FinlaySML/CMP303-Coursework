#pragma once
#include "ClientState.h"
#include <SFML/Network/TcpSocket.hpp>
#include "TickClock.h"
#include "ClientPlayerEntity.h"
#include "PacketFactory.h"
#include "ConnectedSocket.h"
#include <SFML/Graphics/Texture.hpp>
#include "ClientWorld.h"
#include <SFML/Audio.hpp>

class InGame : public ClientState {
    ConnectedSocket server;
    TickClock tickClock;
    ClientWorld world;
    ClientPlayerEntity* localPlayer;
    sf::SoundBuffer minigunSoundSource;
    sf::Sound minigunSound;
    std::vector<sf::Vector2f> bulletHoles;
    sf::Texture bulletHoleDecal;
    sf::Vector2f lastMousePosition;
public:
    InGame(std::unique_ptr<sf::TcpSocket>&& server, PacketFactory::JoinGameData data);
    void ProcessEvent(const sf::Event& event) override {}
    void Update(sf::RenderWindow& window) override;
    void Render(sf::RenderWindow& window) override;
    std::optional<std::unique_ptr<ClientState>> ChangeState() override;
    sf::Color GetClearColor() override;
};