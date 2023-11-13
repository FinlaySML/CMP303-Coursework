#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include "PlayerEntity.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(640, 480), "Topdown Shooter");
    window.setFramerateLimit(60.0f);
    window.setView(sf::View{sf::Vector2f{0,0},sf::Vector2f{16, 12}});
    PlayerEntity player;
    sf::Clock deltaTime;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        player.Update(deltaTime.getElapsedTime().asSeconds(), window);
        deltaTime.restart();
        window.clear(sf::Color{128,64,64});
        window.draw(player);
        window.display();
    }
    return 0;
}