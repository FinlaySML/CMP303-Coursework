#include <SFML/Graphics.hpp>
#include "ConnectToServer.h"

void main() {
    //Window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Topdown Shooter", sf::Style::Default, settings);
    window.setFramerateLimit(60.0f);
    //Connect to server
    std::unique_ptr<ClientState> state{std::make_unique<ConnectToServer>()};
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            state->ProcessEvent(event);
        }
        state->Update(window);
        window.clear(state->GetClearColor());
        state->Render(window);
        window.display();
        auto newState = state->ChangeState();
        if(newState){
            state.swap(newState.value());
        }
    }
}