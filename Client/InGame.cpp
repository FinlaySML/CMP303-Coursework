#include "InGame.h"
#include <iostream>
#include "ConnectToServer.h"
#include <SFML/Graphics/Sprite.hpp>
#include <array>
#include "BarrierEntity.h"
#include "FontManager.h"
#include <format>

InGame::InGame(std::unique_ptr<sf::TcpSocket>&& server, PacketFactory::JoinGameData data) : 
world{ std::move(server), data } {
}

void InGame::ProcessEvent(const sf::Event& event) {
}

void InGame::Update(sf::RenderWindow& window) {
    world.Update(window);
}

void InGame::Render(sf::RenderWindow& window) {
    world.Render(window);
}

std::optional<std::unique_ptr<ClientState>> InGame::ChangeState() {
    if(world.Disconnected()) {
        return std::make_unique<ConnectToServer>();
    }
    return {};
}

sf::Color InGame::GetClearColor() {
    return sf::Color(0xc2a280ff);
}
