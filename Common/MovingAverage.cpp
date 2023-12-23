#include "MovingAverage.h"
#include <cassert>

MovingAverage::MovingAverage(float alpha) :
alpha{ alpha },
average{ 0.0f },
lastValue{ 0 },
initialised{ false } {
}

void MovingAverage::AddValue(int value) {
	lastValue = value;
	if(!initialised) {
		initialised = true;
		average = value;
		return;
	}
	average = average * (1.0f - alpha) + value * alpha;
}

int MovingAverage::GetAverage() const {
	assert(initialised);
	return average;
}

int MovingAverage::GetLastValue() const {
	assert(initialised);
	return lastValue;
}

bool MovingAverage::IsInitialised() const {
	return initialised;
}