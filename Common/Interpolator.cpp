#include "Interpolator.h"
#include <ranges>

Interpolator::Interpolator(int tick, int tickRate, sf::Vector2f position, float rotation) : 
tickRate{tickRate}, 
timePassed{0.0f} {
	ticks[0] = tick - 1;
	ticks[1] = tick;
	keyframes[0] = Keyframe{ position, rotation };
	keyframes[1] = Keyframe{ position, rotation };
}

void Interpolator::AddKeyframe(int tick, sf::Vector2f position, float rotation) {
	if(tick >= ticks[1]) {
		ticks[0] = ticks[1];
		keyframes[0] = keyframes[1];
		ticks[1] = tick;
		keyframes[1] = Keyframe{position, rotation};
		timePassed = 0.0f;
	}
}

float Lerp(float t, float a, float b) {
	float diff{ b - a };
	return a + diff * t;
}

Interpolator::Keyframe Interpolator::GetKeyframe() {
	timePassed += clock.restart().asSeconds() * tickRate;
	float t = std::min(timePassed, tickRate * 0.2f);
	int diff = ticks[1] - ticks[0];
	t /= diff;
	Keyframe result{};
	result.position.x = Lerp(t, keyframes[0].position.x, keyframes[1].position.x);
	result.position.y = Lerp(t, keyframes[0].position.y, keyframes[1].position.y);
	result.rotation = Lerp(t, keyframes[0].rotation, keyframes[1].rotation);
	return result;
}