#include "MovingAverage.h"
#include <vector>
#include <algorithm>
#include <iterator>

MovingAverage::MovingAverage(size_t maxSize) :
maxSize{ maxSize },
median{ 0 } {
}

void MovingAverage::AddValue(int value) {
	buffer.push_back(value);
	if (buffer.size() > maxSize) {
		buffer.pop_front();
	}
	//Sort and get the median
	std::vector<int> sorted(buffer.begin(), buffer.end());
	std::sort(sorted.begin(), sorted.end());
	median = sorted[sorted.size()/2];
}

int MovingAverage::GetAverage() const {
	return median;
}

bool MovingAverage::IsEmpty() const {
	return buffer.size() == 0;
}