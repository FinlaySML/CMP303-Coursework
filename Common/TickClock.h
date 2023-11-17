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
	int GetTick();
	bool ExecuteTick(std::function<void(void)> tickFunction);
};

