#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Clock.hpp>
#include <map>

class Interpolator {
public:
	struct Keyframe {
		sf::Vector2f position;
		float rotation;
	};
private:
	int ticks[2];
	Keyframe keyframes[2];
	int tickRate;
public:
	Interpolator(int tick, int tickRate, sf::Vector2f position, float rotation);
	void AddKeyframe(int tick, sf::Vector2f position, float rotation);
	Keyframe GetKeyframe(int tick);
};

