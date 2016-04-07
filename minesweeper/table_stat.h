#pragma once
#include <set>
#include "sfml.h"

struct round_results {
	float time;
	int hints_used;
};

bool operator < (const round_results & r1, const round_results & r2);

struct table_stat {
	int fails_count;
	int wins_count;
	std::set <round_results> wins;
	table_stat () : fails_count (0), wins_count (0) {}
};

struct table_wind {
	sf::RenderWindow wind;
	bool active;
public:
	table_wind () : active (false) {}
	void show (const table_stat &ts, float time, int realloc_count);
	void close ();
	void update (float dt);
	~table_wind () {if (active) {if (wind.isOpen ()) wind.close ();}};
};