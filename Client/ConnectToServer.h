#pragma once
#include "ClientState.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Network/IpAddress.hpp>

class ConnectToServer : public ClientState {
    sf::Text header;
    sf::Text inputField;
    sf::IpAddress serverAddress;
public:
    ConnectToServer();
    bool EditText(sf::Text& text, sf::Uint32 unicode);
    void ProcessEvent(const sf::Event& event) override;
    void Update(sf::RenderWindow& window) override;
    void Render(sf::RenderWindow& window) override;
    std::optional<std::unique_ptr<ClientState>> ChangeState() override;
    sf::Color GetClearColor() override;
};

