#include "InGame.h"
#include <iostream>
#include "ConnectToServer.h"
#include <SFML/Graphics/Sprite.hpp>
#include <array>

InGame::InGame(std::unique_ptr<sf::TcpSocket>&& server, PacketFactory::JoinGameData data) : 
server{ std::move(server) },
tickClock{ 60 }, 
localPlayer{ data.playerEntityId, data.position, data.rotation, true },
barriers{data.barriers} {
    brickTexture.loadFromFile("brick.png");
}

void InGame::Update(sf::RenderWindow& window) {
    //World View (for getting the world position of the mouse in update)
    window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
    //Tick
    tickClock.ExecuteTick([&]() {
        //Receive Packets
        server.ProcessPackets([&](sf::Packet& packet) {
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
        
        });
        //Local Player Update
        if (window.hasFocus()) {
            PlayerEntity::InputData inputData{ localPlayer.GetInputData(window) };
            localPlayer.Update(tickClock.GetTickDelta(), inputData);
            Collision();
            sf::Packet packet{ PacketFactory::PlayerInput(inputData) };
            server.Send(packet);
        }
    });
}

std::array<sf::Vector2f, 4> directions = {
    sf::Vector2f(-1, 0),
    sf::Vector2f( 1, 0),
    sf::Vector2f( 0,-1),
    sf::Vector2f( 0, 1)
};

void InGame::Collision()
{
    for (int i = 0; i < 2; i++) {
        float overlap = 0.0f;
        size_t mostOverlap = 0;
        for (size_t barrierIndex = 0; barrierIndex < barriers.size(); barrierIndex++) {
            sf::FloatRect barrier{ sf::Vector2f(barriers[barrierIndex]), sf::Vector2f(1,1)};
            sf::FloatRect body{ localPlayer.GetCollisionBox()};
            if (barrier.intersects(body)) {
                float top = std::max(barrier.top, body.top);
                float left = std::max(barrier.left, body.left);
                float bottom = std::min(barrier.top + barrier.height, body.top + body.height);
                float right = std::min(barrier.left + barrier.width, body.left + body.width);
                float thisOverlap = (bottom - top) * (right - left);
                if (thisOverlap > overlap) {
                    overlap = thisOverlap;
                    mostOverlap = barrierIndex;
                }
            }
        }
        if (overlap == 0.0f) {
            break;
        }
        sf::FloatRect barrier{ sf::Vector2f(barriers[mostOverlap]), sf::Vector2f(1,1) };
        sf::FloatRect body{ localPlayer.GetCollisionBox() };
        std::array<float, 4> ejects = {
            std::abs(barrier.left - body.width - body.left), //left
            std::abs(barrier.left + barrier.width - body.left), //right
            std::abs(barrier.top - body.height - body.top), //top
            std::abs(barrier.top + barrier.height - body.top) //bottom
        };
        size_t index = std::distance(ejects.begin(), std::min_element(ejects.begin(), ejects.end()));
        localPlayer.setPosition(localPlayer.getPosition() + directions[index] * ejects[index]);
    }
}

void InGame::Render(sf::RenderWindow& window) {
    sf::Sprite brick{brickTexture};
    brick.setScale(1.0f / 16.0f, 1.0f / 16.0f);
    //Render
    window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
    for (auto& pos : barriers) {
        brick.setPosition(sf::Vector2f{ pos });
        window.draw(brick);
    }
    for (auto& [id, player] : otherPlayers) {
        window.draw(player);
    }
    window.draw(localPlayer);
    //UI
    window.setView(sf::View{ sf::Vector2f{window.getSize()} / 2.0f,sf::Vector2f{window.getSize()} });
}

std::optional<std::unique_ptr<ClientState>> InGame::ChangeState() {
    if(!server.IsConnected()) {
        return std::make_unique<ConnectToServer>();
    }
    return {};
}

sf::Color InGame::GetClearColor() {
    return sf::Color(0xc2a280ff);
}
