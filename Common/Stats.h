#pragma once
#include <cstdint>

struct Stats {
	using TypeUnderlying = std::uint8_t;
	enum class Type : TypeUnderlying {
		KILLS,
		DEATHS,
		HITS,
		MISSES
	};
	Stats();
	int* GetStat(Type type);
	int kills, deaths, hits, misses;
};