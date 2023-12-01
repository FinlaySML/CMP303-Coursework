#include "TickClock.h"

TickClock::TickClock(float ticksPerSecond, int startingTick) : tickDelta{1.0f/ticksPerSecond}, totalTime{0.0f}, tick{ startingTick } {
}

float TickClock::GetTickDelta() {
	return tickDelta;
}

int TickClock::GetTick() {
	return tick;
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
