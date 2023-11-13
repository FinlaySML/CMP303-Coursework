#include <SFML/Network.hpp>
#include <iostream>

int main()
{
    sf::TcpListener listener;
    listener.setBlocking(false);
    if (listener.listen(1234) != sf::Socket::Done) {
        // error...
    }
    while(true) {
        sf::TcpSocket client;
        if(listener.accept(client) == sf::TcpListener::Done) {
        
        }
    }
}