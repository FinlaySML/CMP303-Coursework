#include "ConnectedSocket.h"

ConnectedSocket::ConnectedSocket(sf::TcpListener& listener, ConnectedUDP* udp) : tcp{ std::make_unique<sf::TcpSocket>() }, udp{ udp }, connected{ false } {
    tcp->setBlocking(false);
    if (listener.accept(*tcp) == sf::Socket::Done) {
        connected = true;
    }
}

ConnectedSocket::ConnectedSocket(std::unique_ptr<sf::TcpSocket>&& connectedSocket, ConnectedUDP* udp) : tcp{ std::move(connectedSocket)}, udp{udp}, connected{true} {
}

void ConnectedSocket::ProcessPackets(std::function<void(sf::Packet&)> func) {
    for (sf::Packet packet; tcp->receive(packet) == sf::Socket::Done; packet.clear()) {
        lastReceived.restart();
        func(packet);
    }
}

float ConnectedSocket::TimeSinceLastPacket() const {
    return lastReceived.getElapsedTime().asSeconds();
}

void ConnectedSocket::Send(sf::Packet&& packet, bool reliable) {
    sf::Packet p{packet};
    Send(p, reliable);
}

void ConnectedSocket::Send(sf::Packet& packet, bool reliable) {
    if (reliable) {
        if (tcp->send(packet) == sf::Socket::Disconnected) {
            connected = false;
        }
    } else {
        udp->Send(packet, tcp->getRemoteAddress());
    }
}

void ConnectedSocket::Disconnect() {
    tcp->disconnect();
    connected = false;
}

bool ConnectedSocket::IsConnected() const {
    return connected;
}
