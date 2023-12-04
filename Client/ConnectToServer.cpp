#include "ConnectToServer.h"
#include "ResourceManager.h"
#include "SFML/Window/Event.hpp"
#include <SFML/Network/TcpSocket.hpp>
#include "PacketFactory.h"
#include "InGame.h"

ConnectToServer::ConnectToServer() : 
header{ "Enter the server address:", ResourceManager::GetInstance().arial, 24}, 
inputField{"", ResourceManager::GetInstance().arial, 24},
server{nullptr} {
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
    if (server.get() == nullptr && event.type == sf::Event::TextEntered) {
        if (EditText(inputField, event.text.unicode)) {
            sf::IpAddress ip{ inputField.getString().toAnsiString() };
            if(ip == sf::IpAddress::None) {
                header.setString("Invalid address, try again:");
                inputField.setString("");
            }else{
                server = std::make_unique<ClientNetworking>(ip, DEFAULT_PORT, 2.0f);
            }
        }
    }
}

void ConnectToServer::Update(sf::RenderWindow& window) {
    if (server.get()) {
        header.setString("Attempting to connect...");
    }
}

void ConnectToServer::Render(sf::RenderWindow& window) {
    window.setView(sf::View{ sf::Vector2f{window.getSize()} / 2.0f, sf::Vector2f{window.getSize()} });
    window.draw(header);
    window.draw(inputField);
}

std::optional<std::unique_ptr<ClientState>> ConnectToServer::ChangeState() {
    if (server.get()) {
        if(server->Init()) {
            if(server->GetStatus() == ClientNetworking::Status::CONNECTED) {
                sf::Packet& packet{server->GetFirstPacket()};
                if (PacketFactory::GetType(packet) == PacketType::JOIN_GAME) {
                    return std::make_unique<InGame>(std::move(server), PacketFactory::JoinGame(packet));
                } else {
                    header.setString("Server did not send the correct packet, try again:");
                }
            }else{
                header.setString("Failed to connect, try again:");
            }
            server.reset();
            inputField.setString("");
        }
    }
    return {};
}

sf::Color ConnectToServer::GetClearColor() {
    return sf::Color::Black;
}
