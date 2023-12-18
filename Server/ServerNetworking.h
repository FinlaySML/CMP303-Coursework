#pragma once
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <unordered_map>
#include <functional>
#include "ConnectedClient.h"

class ServerNetworking {
	sf::TcpListener listener;
	sf::UdpSocket udp;
	std::unordered_map<ClientID, std::unique_ptr<ConnectedClient>> clients;
	ClientID usedClientIds;
public:
	ServerNetworking(unsigned short port);
	ConnectedClient* GetNewClient();
	std::vector<std::unique_ptr<ConnectedClient>> GetDisconnectedClients();
	void ProcessPackets(std::function<void(sf::Packet&, ConnectedClient*)> func);
	void Broadcast(sf::Packet packet, ClientID excluding = 0);
	void BroadcastUnreliable(sf::Packet packet, ClientID excluding = 0);
	void Broadcast(const std::string& message);
	std::vector<ConnectedClient*> UpdateRespawnTimer(float dt);
private:
	ClientID GetNewClientID();
};

