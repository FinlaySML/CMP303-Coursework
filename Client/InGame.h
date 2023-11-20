#pragma once
#include "ClientState.h"
#include <SFML/Network/TcpSocket.hpp>
#include "TickClock.h"
#include "ClientPlayerEntity.h"
#include "PacketFactory.h"
#include "ConnectedSocket.h"

class InGame : public ClientState {
    ConnectedSocket server;
    TickClock tickClock;
    std::unordered_map<std::uint16_t, ClientPlayerEntity> otherPlayers;
    ClientPlayerEntity localPlayer;
public:
    InGame(std::unique_ptr<sf::TcpSocket>&& server, PacketFactory::JoinGameData data);
    void ProcessEvent(const sf::Event& event) override {}
    void Update(sf::RenderWindow& window) override;
    void Render(sf::RenderWindow& window) override;
    std::optional<std::unique_ptr<ClientState>> ChangeState() override;
    sf::Color GetClearColor() override;
};