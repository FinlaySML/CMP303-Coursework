#pragma once
#include <deque>
class MovingAverage {
	std::deque<int> buffer;
	size_t maxSize;
	int median;
public:
	MovingAverage(size_t maxSize);
	void AddValue(int value);
	int GetAverage() const;
	bool IsEmpty() const;
};