#pragma once
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include "Entity.h"
#include "Stats.h"
#include "MovingAverage.h"
class ServerPlayerEntity;

using ClientID = std::uint16_t;

class ConnectedClient {
    friend class ServerNetworking;
public:
    enum class Status {
        LOADING,
        PLAYING,
        RESPAWNING,
        DISCONNECTED
    };
private:
    sf::TcpSocket tcp;
    sf::UdpSocket* udp;
    Status status;
    Stats stats;
public:
    ConnectedClient(sf::TcpListener& listener, sf::UdpSocket* udp);
    void Send(sf::Packet packet);
    void SendUnreliable(sf::Packet packet);
    Status GetStatus() const;
    void SetRespawning(float newRespawnTime);
    void SetPlaying(ServerPlayerEntity* newPlayer);
    void IncrementStat(Stats::Type type);
    ClientID id;
    //May be nullptr
    ServerPlayerEntity* player;
    float respawnTime;
    MovingAverage pingTicks;
};

