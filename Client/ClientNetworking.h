#pragma once
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Clock.hpp>
#include <functional>
#include <future>
#include "Stats.h"
#include "MovingAverage.h"

class ClientNetworking {
public:
	enum class Status {
		PENDING_CONNECT,
		PENDING_SET_TICK,
		CONNECTED,
		DISCONNECTED
	};
private:
	sf::TcpSocket tcp;
	sf::UdpSocket udp;
	Status status;
	//Establishing connection
	std::future<sf::Socket::Status> pending;
	sf::Packet firstPacket;
	int serverTick;
public:
	ClientNetworking(sf::IpAddress serverIp, unsigned short serverPort, float waitTime);
	//Returns true when the initialisation is finished
	bool Init();
	void ProcessPackets(std::function<void(sf::Packet&)> func);
	void Send(sf::Packet packet);
	void SendUnreliable(sf::Packet packet);
	Status GetStatus() const;
	int GetServerTick() const;
	Stats stats;
	int rtt;
};

