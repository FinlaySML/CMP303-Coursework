#include "Stats.h"

Stats::Stats() : 
kills{0}, 
deaths{0}, 
hits{0}, 
misses{0} {
}

int* Stats::GetStat(Type type) {
	switch (type) {
	case Stats::Type::KILLS: return &kills;
	case Stats::Type::DEATHS: return &deaths;
	case Stats::Type::HITS: return &hits;
	case Stats::Type::MISSES: return &misses;
	default: return nullptr;
	}
}