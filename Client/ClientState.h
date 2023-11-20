#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include <optional>

class ClientState {
public:
    virtual ~ClientState();
    virtual void ProcessEvent(const sf::Event& event) = 0;
    virtual void Update(sf::RenderWindow& window) = 0;
    virtual void Render(sf::RenderWindow& window) = 0;
    virtual std::optional<std::unique_ptr<ClientState>> ChangeState() = 0;
    virtual sf::Color GetClearColor() = 0;
};