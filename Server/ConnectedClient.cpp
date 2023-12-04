#include "ConnectedClient.h"
#include <SFML/Network/TcpListener.hpp>

ConnectedClient::ConnectedClient(sf::TcpListener& listener, sf::UdpSocket* udp) : udp{udp}, id{0}, player{nullptr}, status{Status::DISCONNECTED} {
	tcp.setBlocking(false);
	if(listener.accept(tcp) == sf::Socket::Done) {
		status = Status::CONNECTED;
	}
}

void ConnectedClient::Send(sf::Packet packet, bool reliable) {
	if(reliable) {
		if(tcp.send(packet) == sf::Socket::Disconnected){
			status = Status::DISCONNECTED;
		}
	}else{
		udp->send(packet, tcp.getRemoteAddress(), tcp.getRemotePort());
	}
}

ConnectedClient::Status ConnectedClient::GetStatus() const {
	return status;
}
