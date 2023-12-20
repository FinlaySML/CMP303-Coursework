#pragma once
class MovingAverage {
	float alpha;
	float average;
	bool initialised;
public:
	MovingAverage(float alpha);
	void AddValue(int value);
	int GetAverage() const;
	bool IsInitialised() const;
};