#pragma once
#include <memory>
#include <functional>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>

class ConnectedUDP {
    std::unique_ptr<sf::UdpSocket> udp;
    sf::Clock lastReceived;
    bool connected;
public:
    ConnectedUDP(unsigned short port);
    void ProcessPackets(std::function<void(sf::Packet&, sf::IpAddress&)> func);
    float TimeSinceLastPacket() const;
    void Send(sf::Packet&& packet, sf::IpAddress ip);
    void Send(sf::Packet& packet, sf::IpAddress ip);
    bool IsConnected() const;
};

