#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <PacketFactory.h>
#include <TickClock.h>
#include "ClientPlayerEntity.h"

std::unordered_map<std::uint16_t, ClientPlayerEntity> otherPlayers;

void main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Topdown Shooter", sf::Style::Default, settings);
    window.setFramerateLimit(60.0f);
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return;
    }
    //Connect to server
    sf::Text header("Enter the server address:", font, 24);
    sf::Text inputField("", font, 24);
    inputField.setPosition({0, 24});
    sf::TcpSocket server;
    sf::IpAddress serverAddress;
    PacketFactory::JoinGameData joinGameData;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (event.type == sf::Event::TextEntered) {
                sf::String s = inputField.getString();
                if(event.text.unicode == '\r') {
                    serverAddress = s.toAnsiString();
                } else if (event.text.unicode == '\b') {
                    if (s.getSize() > 0) {
                        s.erase(s.getSize() - 1);
                    }
                } else {
                    s += event.text.unicode;
                }
                inputField.setString(s);
            }
        }
        if(serverAddress != sf::IpAddress::None) {
            header.setString("Attempting to connect...");
        }
        window.clear(sf::Color{ 128,128,128 });
        window.setView(sf::View{ sf::Vector2f{window.getSize()} / 2.0f,sf::Vector2f{window.getSize()} });
        window.draw(header);
        window.draw(inputField);
        window.display();
        if(serverAddress != sf::IpAddress::None) {
            if (server.connect(serverAddress, DEFAULT_PORT, sf::seconds(2.0f)) == sf::Socket::Done) {
                sf::Packet packet;
                server.receive(packet);
                if (PacketFactory::GetType(packet) == PacketFactory::PacketType::JOIN_GAME) {
                    joinGameData = PacketFactory::JoinGame(packet);
                    break;
                }else{
                    header.setString("Server did not send the correct packet, try again:");
                }
            }else{
                header.setString("Failed to connect, try again:");
            }
            serverAddress = sf::IpAddress::None;
            inputField.setString("");
        }
    }
    ClientPlayerEntity localPlayer{ joinGameData.playerEntityId, joinGameData.position, joinGameData.rotation, true };
    server.setBlocking(false);
    //Play Game
    TickClock tickClock{60};
    while (window.isOpen()) {
        //Poll Event
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        //World View (for getting the world position of the mouse in update)
        window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
        //Tick
        tickClock.ExecuteTick([&](){
            //Receive Packets
            for (sf::Packet packet; server.receive(packet) == sf::TcpSocket::Done; packet.clear()) {
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
            }
            //Local Player Update
            if (window.hasFocus()) {
                PlayerEntity::InputData inputData{ localPlayer.GetInputData(window) };
                localPlayer.Update(tickClock.GetTickDelta(), inputData);
                sf::Packet packet{ PacketFactory::PlayerInput(inputData) };
                server.send(packet);
            }
        });
        //Render
        window.clear(sf::Color{128,64,64});
        window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
        for (auto& [id, player] : otherPlayers) {
            window.draw(player);
        }
        window.draw(localPlayer);
        //UI View
        window.setView(sf::View{sf::Vector2f{window.getSize()}/2.0f,sf::Vector2f{window.getSize()}});
        window.display();
    }
}