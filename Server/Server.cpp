#include <SFML/Network.hpp>
#include <iostream>

void main()
{
    sf::TcpListener listener;
    if (listener.listen(1234) != sf::Socket::Done) {
        return;
    }
    sf::TcpSocket client;
    if(listener.accept(client) != sf::TcpListener::Done) {
        return;
    }
    sf::Packet packet;
    packet << "Fart";
    client.send(packet);
}