#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <PacketFactory.h>
#include <TickClock.h>
#include "ClientPlayerEntity.h"

sf::Font arial;

class GameState {
public:
    virtual void ProcessEvent(const sf::Event& event) = 0;
    virtual void Update(sf::RenderWindow& window) = 0;
    virtual void Render(sf::RenderWindow& window) = 0;
    virtual std::optional<std::unique_ptr<GameState>> ChangeState() = 0;
};

class InGame : public GameState {
    std::unique_ptr<sf::TcpSocket> server;
    TickClock tickClock;
    std::unordered_map<std::uint16_t, ClientPlayerEntity> otherPlayers;
    ClientPlayerEntity localPlayer;
public:
    InGame(std::unique_ptr<sf::TcpSocket>&& server, PacketFactory::JoinGameData data) : server{std::move(server)}, tickClock{60}, localPlayer{data.playerEntityId, data.position, data.rotation, true} {
    }

    void ProcessEvent(const sf::Event& event) override {
    }

    void Update(sf::RenderWindow& window) override {
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
                if(type == PacketFactory::PacketType::MESSAGE) {
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

    void Render(sf::RenderWindow& window) override {
        //Render
        window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
        for (auto& [id, player] : otherPlayers) {
            window.draw(player);
        }
        window.draw(localPlayer);
        //UI
        window.setView(sf::View{ sf::Vector2f{window.getSize()} / 2.0f,sf::Vector2f{window.getSize()} });
    }

    std::optional<std::unique_ptr<GameState>> ChangeState() {
        return {};
    }
};

class ConnectToServer : public GameState {
    sf::Text header;
    sf::Text inputField;
    sf::IpAddress serverAddress;
public:
    ConnectToServer() : header{ "Enter the server address:", arial, 24 }, inputField{ "", arial, 24 } {
        inputField.setPosition({ 0, 24 });
    }

    bool EditText(sf::Text& text, sf::Uint32 unicode) {
        sf::String s = text.getString();
        if (unicode == '\r') {
            return true;
        }
        if (unicode == '\b') {
            if (s.getSize() > 0) {
                s.erase(s.getSize() - 1);
            }
        } else {
            s += unicode;
        }
        text.setString(s);
        return false;
    }

    void ProcessEvent(const sf::Event& event) override {
        if (event.type == sf::Event::TextEntered) {
            if(EditText(inputField, event.text.unicode)) {
                serverAddress = inputField.getString().toAnsiString();
            }
        }
    }

    void Update(sf::RenderWindow& window) override {
        if (serverAddress != sf::IpAddress::None) {
            header.setString("Attempting to connect...");
        }
    }

    void Render(sf::RenderWindow& window) override {
        window.setView(sf::View{ sf::Vector2f{window.getSize()} / 2.0f, sf::Vector2f{window.getSize()} });
        window.draw(header);
        window.draw(inputField);
    }

    std::optional<std::unique_ptr<GameState>> ChangeState() override {
        std::unique_ptr<sf::TcpSocket> server{std::make_unique<sf::TcpSocket>()};
        if (serverAddress != sf::IpAddress::None) {
            if (server->connect(serverAddress, DEFAULT_PORT, sf::seconds(2.0f)) == sf::Socket::Done) {
                sf::Packet packet;
                server->receive(packet);
                if (PacketFactory::GetType(packet) == PacketFactory::PacketType::JOIN_GAME) {
                    server->setBlocking(false);
                    return std::make_unique<InGame>(std::move(server), PacketFactory::JoinGame(packet));
                } else {
                    header.setString("Server did not send the correct packet, try again:");
                }
            } else {
                header.setString("Failed to connect, try again:");
            }
            serverAddress = sf::IpAddress::None;
            inputField.setString("");
        }
        return {};
    }
};

void main() {
    //Window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Topdown Shooter", sf::Style::Default, settings);
    window.setFramerateLimit(60.0f);
    //Font
    arial.loadFromFile("arial.ttf");
    //Connect to server
    std::unique_ptr<GameState> state{std::make_unique<ConnectToServer>()};
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            state->ProcessEvent(event);
        }
        state->Update(window);
        window.clear(sf::Color{ 128,128,128 });
        state->Render(window);
        window.display();
        auto newState = state->ChangeState();
        if(newState){
            state.swap(newState.value());
        }
    }
}