#pragma once
#include <memory>
#include <functional>
#include <unordered_map>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/System/Clock.hpp>

struct ConnectedSocket {
    std::unique_ptr<sf::TcpSocket> socket;
    sf::Clock lastReceived;
    bool connected;
    ConnectedSocket(sf::TcpListener& listener) : socket{std::make_unique<sf::TcpSocket>()}, connected{false} {
        socket->setBlocking(false);
        if (listener.accept(*socket) == sf::TcpListener::Done) {
            connected = true;
        }
    }
    void ProcessPackets(std::function<void(sf::Packet&)> func) {
        for (sf::Packet packet; socket->receive(packet) == sf::TcpSocket::Done; packet.clear()) {
            lastReceived.restart();
            func(packet);
        }
    }
    float TimeSinceLastPacket() {
        return lastReceived.getElapsedTime().asSeconds();
    }
    void Send(sf::Packet& packet) {
        socket->send(packet);
    }
    bool IsConnected() {
        return connected;
    }
    static void SendToAll(std::unordered_map<std::uint16_t, ConnectedSocket>& map, sf::Packet& packet) {
        for (auto& [key, value] : map) {
            value.Send(packet);
        }
    }
};