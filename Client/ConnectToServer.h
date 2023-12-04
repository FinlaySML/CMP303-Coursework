#pragma once
#include "ClientState.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Network/IpAddress.hpp>
#include "ClientNetworking.h"

class ConnectToServer : public ClientState {
    sf::Text header;
    sf::Text inputField;
    std::unique_ptr<ClientNetworking> server;
public:
    ConnectToServer();
    bool EditText(sf::Text& text, sf::Uint32 unicode);
    void ProcessEvent(const sf::Event& event) override;
    void Update(sf::RenderWindow& window) override;
    void Render(sf::RenderWindow& window) override;
    std::optional<std::unique_ptr<ClientState>> ChangeState() override;
    sf::Color GetClearColor() override;
};

