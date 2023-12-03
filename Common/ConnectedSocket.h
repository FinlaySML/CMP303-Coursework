#pragma once
#include <memory>
#include <functional>
#include <unordered_map>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>
#include "ConnectedUDP.h"

class ConnectedSocket {
    std::unique_ptr<sf::TcpSocket> tcp;
    ConnectedUDP* udp;
    sf::Clock lastReceived;
    bool connected;
public:
    ConnectedSocket(sf::TcpListener& listener, ConnectedUDP* udp);
    ConnectedSocket(std::unique_ptr<sf::TcpSocket>&& connectedSocket, ConnectedUDP* udp);
    void ProcessPackets(std::function<void(sf::Packet&)> func);
    float TimeSinceLastPacket() const;
    void Send(sf::Packet&& packet, bool reliable = true);
    void Send(sf::Packet& packet, bool reliable = true);
    void Disconnect();
    bool IsConnected() const;
};