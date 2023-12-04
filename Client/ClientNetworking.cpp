#include "ClientNetworking.h"
#include <iostream>

ClientNetworking::ClientNetworking(sf::IpAddress serverIp, unsigned short serverPort, float waitTime) : 
	pending{std::async(std::launch::async, &sf::TcpSocket::connect, &tcp, serverIp, serverPort, sf::seconds(waitTime))},
	status{Status::PENDING_CONNECT} {
}

bool ClientNetworking::Init() {
	switch(status) {
		case Status::PENDING_CONNECT:{
			if (pending.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
				return false;
			}
			if (pending.get() == sf::Socket::Done) {
				status = Status::PENDING_FIRST_PACKET;
				udp.bind(tcp.getLocalPort());
				pending = std::async(std::launch::async, [&] { return tcp.receive(firstPacket); });
				return false;
			} else {
				status = Status::DISCONNECTED;
			}
			break;
		}
		case Status::PENDING_FIRST_PACKET:{
			if (pending.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
				return false;
			}
			tcp.setBlocking(false);
			udp.setBlocking(false);
			if (pending.get() == sf::Socket::Done) {
				status = Status::CONNECTED;
				std::cout << "TCP: " << tcp.getLocalPort() << " -> " << tcp.getRemoteAddress() << " " << tcp.getRemotePort() << std::endl;
				std::cout << "UDP: " << udp.getLocalPort() << std::endl;
			} else {
				status = Status::DISCONNECTED;
			}
			break;
		}
	}
	return true;
}

sf::Packet& ClientNetworking::GetFirstPacket() {
	return firstPacket;
}

void ClientNetworking::ProcessPackets(std::function<void(sf::Packet&)> func) {
	for (sf::Packet packet; tcp.receive(packet) == sf::Socket::Done; packet.clear()) {
		func(packet);
	}
	sf::IpAddress serverIp;
	unsigned short serverPort;
	for (sf::Packet packet; udp.receive(packet, serverIp, serverPort) == sf::Socket::Done; packet.clear()) {
		if(tcp.getRemoteAddress() == serverIp && tcp.getRemotePort() == serverPort) {
			func(packet);
		}
	}
}

void ClientNetworking::Send(sf::Packet packet, bool reliable) {
	if (reliable) {
		if (tcp.send(packet) == sf::Socket::Disconnected) {
			status = Status::DISCONNECTED;
		}
	} else {
		udp.send(packet, tcp.getRemoteAddress(), tcp.getRemotePort());
	}
}

ClientNetworking::Status ClientNetworking::GetStatus() const {
	return status;
}
