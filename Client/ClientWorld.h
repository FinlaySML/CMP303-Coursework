#pragma once
#include <World.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

class ClientWorld : public World {
public:
	ClientWorld();
	void Render(sf::RenderWindow& window);
private:
	sf::Texture brickTexture;
};

