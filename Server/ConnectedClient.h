#pragma once
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include "Entity.h"
class ServerPlayerEntity;

using ClientID = std::uint16_t;

class ConnectedClient {
    friend class ServerNetworking;
public:
    enum class Status {
        CONNECTED,
        DISCONNECTED
    };
private:
    sf::TcpSocket tcp;
    sf::UdpSocket* udp;
    Status status;
public:
    ConnectedClient(sf::TcpListener& listener, sf::UdpSocket* udp);
    void Send(sf::Packet packet, bool reliable = true);
    Status GetStatus() const;
    ClientID id;
    //May be nullptr
    ServerPlayerEntity* player;
};

