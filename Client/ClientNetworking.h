#pragma once
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <functional>
#include <future>

class ClientNetworking {
public:
	enum class Status {
		PENDING_CONNECT,
		PENDING_FIRST_PACKET,
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
public:
	ClientNetworking(sf::IpAddress serverIp, unsigned short serverPort, float waitTime);
	//Returns true when the initialisation is finished
	bool Init();
	sf::Packet& GetFirstPacket();
	void ProcessPackets(std::function<void(sf::Packet&)> func);
	void Send(sf::Packet packet, bool reliable = true);
	Status GetStatus() const;
};

