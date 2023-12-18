#include "ConnectedClient.h"
#include <SFML/Network/TcpListener.hpp>
#include <cassert>
#include "PacketFactory.h"
#include "ServerPlayerEntity.h"

ConnectedClient::ConnectedClient(sf::TcpListener& listener, sf::UdpSocket* udp) : 
udp{udp}, 
id{0}, 
player{nullptr},
respawnTime{0.0f},
status{Status::DISCONNECTED} {
	tcp.setBlocking(false);
	if(listener.accept(tcp) == sf::Socket::Done) {
		status = Status::LOADING;
	}
}

void ConnectedClient::Send(sf::Packet packet) {
	if (tcp.send(packet) == sf::Socket::Disconnected) {
		status = Status::DISCONNECTED;
	}
}

void ConnectedClient::SendUnreliable(sf::Packet packet) {
	udp->send(packet, tcp.getRemoteAddress(), tcp.getRemotePort());
}

ConnectedClient::Status ConnectedClient::GetStatus() const {
	return status;
}

void ConnectedClient::SetRespawning(float newRespawnTime) {
	status = ConnectedClient::Status::RESPAWNING;
	respawnTime = newRespawnTime;
	player = nullptr;
	Send(PacketFactory::ModeRespawning(newRespawnTime));
}

void ConnectedClient::SetPlaying(ServerPlayerEntity* newPlayer) {
	status = ConnectedClient::Status::PLAYING;
	player = newPlayer;
	respawnTime = 0.0f;
	Send(PacketFactory::ModePlaying(newPlayer->GetID()));
}

void ConnectedClient::IncrementStat(Stats::Type type) {
	int* statPtr(stats.GetStat(type));
	if(statPtr) {
		(*statPtr) += 1;
		Send(PacketFactory::StatUpdate(type, *statPtr));
	}
}
