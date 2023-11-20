#include "ConnectedSocket.h"

ConnectedSocket::ConnectedSocket(sf::TcpListener& listener) : socket{ std::make_unique<sf::TcpSocket>() }, connected{ false } {
    socket->setBlocking(false);
    if (listener.accept(*socket) == sf::TcpListener::Done) {
        connected = true;
    }
}

ConnectedSocket::ConnectedSocket(std::unique_ptr<sf::TcpSocket>&& connectedSocket) : socket{ std::move(connectedSocket)}, connected{true} {
}

void ConnectedSocket::ProcessPackets(std::function<void(sf::Packet&)> func) {
    for (sf::Packet packet; socket->receive(packet) == sf::TcpSocket::Done; packet.clear()) {
        lastReceived.restart();
        func(packet);
    }
}

float ConnectedSocket::TimeSinceLastPacket() const {
    return lastReceived.getElapsedTime().asSeconds();
}

void ConnectedSocket::Send(sf::Packet& packet) {
    if (socket->send(packet) == sf::Socket::Disconnected) {
        connected = false;
    }
}

void ConnectedSocket::Disconnect() {
    socket->disconnect();
    connected = false;
}

bool ConnectedSocket::IsConnected() const {
    return connected;
}
