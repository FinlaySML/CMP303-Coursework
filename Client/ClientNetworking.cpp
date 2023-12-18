#include "ClientNetworking.h"
#include <iostream>
#include "PacketFactory.h"

ClientNetworking::ClientNetworking(sf::IpAddress serverIp, unsigned short serverPort, float waitTime) : 
	pending{std::async(std::launch::async, &sf::TcpSocket::connect, &tcp, serverIp, serverPort, sf::seconds(waitTime))},
	status{Status::PENDING_CONNECT},
	ping{0},
	serverTick{0} {
}

bool ClientNetworking::Init() {
	switch(status) {
		case Status::PENDING_CONNECT:{
			if (pending.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
				return false;
			}
			if (pending.get() == sf::Socket::Done) {
				status = Status::PENDING_PONG;
				udp.bind(tcp.getLocalPort());
				Send(PacketFactory::Ping());
				pingClock.restart();
				pending = std::async(std::launch::async, [&] { return tcp.receive(firstPacket); });
				return false;
			} else {
				status = Status::DISCONNECTED;
			}
			break;
		}
		case Status::PENDING_PONG:{
			if (pending.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
				return false;
			}
			tcp.setBlocking(false);
			udp.setBlocking(false);
			if (pending.get() == sf::Socket::Done && PacketFactory::GetType(firstPacket) == PacketType::PONG) {
				status = Status::CONNECTED;
				ping = pingClock.restart().asMilliseconds() / 2;
				serverTick = PacketFactory::Pong(firstPacket);
				std::cout << "TCP: " << tcp.getLocalPort() << " -> " << tcp.getRemoteAddress() << " " << tcp.getRemotePort() << std::endl;
				std::cout << "UDP: " << udp.getLocalPort() << std::endl;
				std::cout << "PING: " << ping << std::endl;
			} else {
				status = Status::DISCONNECTED;
			}
			break;
		}
	}
	return true;
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

void ClientNetworking::Send(sf::Packet packet) {
	if (tcp.send(packet) == sf::Socket::Disconnected) {
		status = Status::DISCONNECTED;
	}
}

void ClientNetworking::SendUnreliable(sf::Packet packet) {
	udp.send(packet, tcp.getRemoteAddress(), tcp.getRemotePort());
}

ClientNetworking::Status ClientNetworking::GetStatus() const {
	return status;
}

int ClientNetworking::GetPing() const {
	return ping;
}

int ClientNetworking::GetServerTick() const {
	return serverTick;
}
