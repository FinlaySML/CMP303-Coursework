#pragma once
#include <SFML/System/Clock.hpp>
#include <functional>

class TickClock {
	sf::Clock clock;
	float totalTime;
	float tickDelta;
	int tick;
public:
	TickClock(float ticksPerSecond, int startingTick = 0);
	float GetTickDelta() const;
	int GetTick() const;
	void SetTick(int newTarget);
	bool ExecuteTick(std::function<void(void)> tickFunction);
};

