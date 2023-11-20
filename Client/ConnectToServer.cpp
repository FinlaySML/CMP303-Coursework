#include "ConnectToServer.h"
#include "FontManager.h"
#include "SFML/Window/Event.hpp"
#include <SFML/Network/TcpSocket.hpp>
#include "PacketFactory.h"
#include "InGame.h"

ConnectToServer::ConnectToServer() : header{ "Enter the server address:", FontManager::GetArial(), 24}, inputField{"", FontManager::GetArial(), 24} {
    inputField.setPosition({ 0, 24 });
}

bool ConnectToServer::EditText(sf::Text& text, sf::Uint32 unicode) {
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

void ConnectToServer::ProcessEvent(const sf::Event& event) {
    if (event.type == sf::Event::TextEntered) {
        if (EditText(inputField, event.text.unicode)) {
            serverAddress = inputField.getString().toAnsiString();
        }
    }
}

void ConnectToServer::Update(sf::RenderWindow& window) {
    if (serverAddress != sf::IpAddress::None) {
        header.setString("Attempting to connect...");
    }
}

void ConnectToServer::Render(sf::RenderWindow& window) {
    window.setView(sf::View{ sf::Vector2f{window.getSize()} / 2.0f, sf::Vector2f{window.getSize()} });
    window.draw(header);
    window.draw(inputField);
}

std::optional<std::unique_ptr<ClientState>> ConnectToServer::ChangeState() {
    std::unique_ptr<sf::TcpSocket> server{ std::make_unique<sf::TcpSocket>() };
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

sf::Color ConnectToServer::GetClearColor() {
    return sf::Color::Black;
}
