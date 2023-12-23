#pragma once
class MovingAverage {
	float alpha;
	float average;
	int lastValue;
	bool initialised;
public:
	MovingAverage(float alpha);
	void AddValue(int value);
	int GetAverage() const;
	int GetLastValue() const;
	bool IsInitialised() const;
};