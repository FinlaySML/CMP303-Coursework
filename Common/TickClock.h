#pragma once
#include <SFML/System/Clock.hpp>
#include <functional>

class TickClock {
	sf::Clock clock;
	float totalTime;
	float tickDelta;
	int tick;
public:
	TickClock(float ticksPerSecond);
	float GetTickDelta();
	bool ExecuteTick(std::function<void(void)> tickFunction);
};

