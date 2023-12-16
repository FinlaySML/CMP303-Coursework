#include "TickClock.h"
#include <iostream>
#include <format>

const int MAX_EXECUTES{ 5 };

TickClock::TickClock(float ticksPerSecond, int startingTick) : 
tickDelta{ 1.0f / ticksPerSecond }, 
totalTime{ 0.0f }, 
tick{ startingTick } {
}

float TickClock::GetTickDelta() const {
	return tickDelta;
}

int TickClock::GetTick() const {
	return tick;
}

void TickClock::SetTick(int newTick) {
	tick = newTick;
}

bool TickClock::ExecuteTick(std::function<void(void)> tickFunction) {
	int executed{0};
	totalTime += clock.restart().asSeconds();
	while(totalTime > tickDelta) {
		totalTime -= tickDelta;
		if(executed < MAX_EXECUTES) {
			tickFunction();
		}
		executed++;
		tick++;
	}
	if(executed > MAX_EXECUTES) {
		std::cout << std::format("Skipped {} ticks", executed - MAX_EXECUTES) << std::endl;
	}
	return executed > 0;
}
