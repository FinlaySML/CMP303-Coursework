#include "ConnectedUDP.h"
#include <SFML/Network/Packet.hpp>

ConnectedUDP::ConnectedUDP(unsigned short port) : connected{false}, udp{std::make_unique<sf::UdpSocket>()} {
	udp->setBlocking(false);
	if (udp->bind(port) == sf::Socket::Done) {
		connected = true;
	}
}

void ConnectedUDP::ProcessPackets(std::function<void(sf::Packet&,sf::IpAddress&)> func) {
	sf::IpAddress ip;
	unsigned short port;
	for (sf::Packet packet; udp->receive(packet, ip, port) == sf::Socket::Done; packet.clear()) {
		lastReceived.restart();
		func(packet, ip);
	}
}

float ConnectedUDP::TimeSinceLastPacket() const {
	return lastReceived.getElapsedTime().asSeconds();
}

void ConnectedUDP::Send(sf::Packet&& packet, sf::IpAddress ip) {
	sf::Packet p{packet};
	Send(p, ip);
}

void ConnectedUDP::Send(sf::Packet& packet, sf::IpAddress ip) {
	udp->send(packet, ip, udp->getLocalPort());
}

bool ConnectedUDP::IsConnected() const {
	return false;
}
