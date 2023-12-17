#include "Interpolator.h"
#include <ranges>
#include <algorithm>

const float INTERPOLATION_LAG{0.1f};//100 ms

Interpolator::Interpolator(int tick, int tickRate, sf::Vector2f position, float rotation) : 
tickRate{tickRate} {
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
	}
}

float Lerp(float t, float a, float b) {
	float diff{ b - a };
	return a + diff * t;
}

Interpolator::Keyframe Interpolator::GetKeyframe(int tick) {
	tick -= INTERPOLATION_LAG * tickRate;
	float t = (tick - ticks[0]) / (float)(ticks[1] - ticks[0]);
	t = std::clamp(t, -0.5f, 1.5f);
	Keyframe result{};
	result.position.x = Lerp(t, keyframes[0].position.x, keyframes[1].position.x);
	result.position.y = Lerp(t, keyframes[0].position.y, keyframes[1].position.y);
	float rotDiff = keyframes[1].rotation - keyframes[0].rotation;
	if(rotDiff > 180) {
		rotDiff -= 360;
	} else if(rotDiff < -180) {
		rotDiff += 360;
	}
	float rotResult = Lerp(t, keyframes[0].rotation, keyframes[0].rotation + rotDiff);
	if(rotResult < 0) {
		rotResult += 360;
	}else if(rotResult > 360) {
		rotResult -= 360;
	}
	result.rotation = rotResult;
	return result;
}