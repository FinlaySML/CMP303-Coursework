#pragma once
#include "ClientState.h"
#include "ClientWorld.h"

class InGame : public ClientState {
    ClientWorld world;
public:
    InGame(std::unique_ptr<sf::TcpSocket>&& server, PacketFactory::JoinGameData data);
    void ProcessEvent(const sf::Event& event) override;
    void Update(sf::RenderWindow& window) override;
    void Render(sf::RenderWindow& window) override;
    std::optional<std::unique_ptr<ClientState>> ChangeState() override;
    sf::Color GetClearColor() override;
};