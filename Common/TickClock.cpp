#include "TickClock.h"

TickClock::TickClock(float ticksPerSecond) : tickDelta{1.0f/ticksPerSecond}, totalTime{0.0f}, tick{0} {
}

float TickClock::GetTickDelta() {
	return tickDelta;
}

bool TickClock::ExecuteTick(std::function<void(void)> tickFunction) {
	bool executed{false};
	totalTime += clock.restart().asSeconds();
	while(totalTime > tickDelta) {
		totalTime -= tickDelta;
		tickFunction();
		executed = true;
		tick++;
	}
	return executed;
}
