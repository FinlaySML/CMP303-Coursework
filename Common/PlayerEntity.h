#pragma once
#include <SFML/Graphics.hpp>

class PlayerEntity : public sf::Transformable {
public:
	struct InputData {
		bool w, a, s, d;
		float rotation;
	};
	PlayerEntity(std::uint16_t id, sf::Vector2f position, float rotation);
	void Update(sf::Vector2f position, float rotation);
	void Update(float deltaTime, InputData inputData);
	sf::FloatRect GetCollisionBox() const;
	const std::uint16_t id;
};