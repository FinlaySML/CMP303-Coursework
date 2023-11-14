#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include "PlayerEntity.h"

void main() {
    sf::RenderWindow window(sf::VideoMode(640, 480), "Topdown Shooter");
    window.setFramerateLimit(60.0f);

    sf::TcpSocket server;
    sf::Socket::Status status = server.connect("127.0.0.1", 1234, sf::seconds(1.0f));
    if (status != sf::Socket::Done) {
        return;
    }
    sf::Packet packet;
    server.receive(packet);
    std::string message;
    packet >> message;
    std::cout << message << std::endl;
    PlayerEntity player;
    sf::String playerInput;
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return;
    }
    sf::Text playerText;
    playerText.setFont(font); // font is a sf::Font
    playerText.setCharacterSize(24); // in pixels, not points!
    sf::Clock deltaTime;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8) {
                    if(playerInput.getSize() > 0) {
                        playerInput.erase(playerInput.getSize() - 1);
                    }
                }else{
                    playerInput += event.text.unicode;
                }
                playerText.setString(playerInput);
            }
        }
        player.Update(deltaTime.getElapsedTime().asSeconds(), window);
        deltaTime.restart();
        window.clear(sf::Color{128,64,64});
        //World View
        window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
        window.draw(player);
        //UI View
        window.setView(sf::View{sf::Vector2f{window.getSize()}/2.0f,sf::Vector2f{window.getSize()}});
        window.draw(playerText);
        window.display();
    }
    return;
}