#pragma once
#include <memory>
#include <functional>
#include <unordered_map>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>

class ConnectedSocket {
    std::unique_ptr<sf::TcpSocket> socket;
    sf::Clock lastReceived;
    bool connected;
public:
    ConnectedSocket(sf::TcpListener& listener);
    ConnectedSocket(std::unique_ptr<sf::TcpSocket>&& connectedSocket);
    void ProcessPackets(std::function<void(sf::Packet&)> func);
    float TimeSinceLastPacket() const;
    void Send(sf::Packet& packet);
    void Disconnect();
    bool IsConnected() const;
};