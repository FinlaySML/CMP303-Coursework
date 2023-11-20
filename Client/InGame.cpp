#include "InGame.h"
#include <iostream>

InGame::InGame(std::unique_ptr<sf::TcpSocket>&& server, PacketFactory::JoinGameData data) : 
server{ std::move(server) }, 
tickClock{ 60 }, 
localPlayer{ data.playerEntityId, data.position, data.rotation, true } {}

void InGame::Update(sf::RenderWindow& window) {
    //World View (for getting the world position of the mouse in update)
    window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
    //Tick
    tickClock.ExecuteTick([&]() {
        //Receive Packets
        for (sf::Packet packet; server->receive(packet) == sf::TcpSocket::Done; packet.clear()) {
            PacketFactory::PacketType type{ PacketFactory::GetType(packet) };
            if (type == PacketFactory::PacketType::PLAYER_UPDATE) {
                auto updateData = PacketFactory::PlayerUpdate(packet);
                for (const auto& data : updateData) {
                    if (data.entityId == localPlayer.id) {
                        continue;
                    }
                    auto it = otherPlayers.find(data.entityId);
                    if (it == otherPlayers.end()) {
                        otherPlayers.insert({ data.entityId, {data.entityId, data.position, data.rotation} });
                    } else {
                        it->second.Update(data.position, data.rotation);
                    }
                }
            }
            if (type == PacketFactory::PacketType::MESSAGE) {
                std::string message = PacketFactory::Message(packet);
                std::cout << message << std::endl;
            }
        }
        //Local Player Update
        if (window.hasFocus()) {
            PlayerEntity::InputData inputData{ localPlayer.GetInputData(window) };
            localPlayer.Update(tickClock.GetTickDelta(), inputData);
            sf::Packet packet{ PacketFactory::PlayerInput(inputData) };
            server->send(packet);
        }
    });
}

void InGame::Render(sf::RenderWindow& window) {
    //Render
    window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
    for (auto& [id, player] : otherPlayers) {
        window.draw(player);
    }
    window.draw(localPlayer);
    //UI
    window.setView(sf::View{ sf::Vector2f{window.getSize()} / 2.0f,sf::Vector2f{window.getSize()} });
}

std::optional<std::unique_ptr<ClientState>> InGame::ChangeState() {
    return {};
}

sf::Color InGame::GetClearColor() {
    return sf::Color(165, 82, 82);
}
