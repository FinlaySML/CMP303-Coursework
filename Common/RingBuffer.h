#pragma once
#include <array>
#include <cassert>

template <class T, size_t N>
class RingBuffer {
	std::array<T, N> data;
	size_t start;
	size_t used;
public:
	RingBuffer();
	size_t size() const;
	void push(T value);
	T& operator[](size_t index);
};

template<class T, size_t N>
inline RingBuffer<T, N>::RingBuffer() : start{0}, used{0} {
}

template<class T, size_t N>
inline size_t RingBuffer<T, N>::size() const {
	return used;
}

template<class T, size_t N>
inline void RingBuffer<T, N>::push(T value) {
	if(used == N) {
		data[start] = value;
		start = (start + 1) % N;
	}else{	
		assert(start == 0);
		data[used] = value;
		used++;
	}
}

template<class T, size_t N>
inline T& RingBuffer<T, N>::operator[](size_t index) {
	assert(index >= 0 && index < used);
	return data[(start + index) % N];
}
