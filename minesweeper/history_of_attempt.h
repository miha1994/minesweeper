#pragma once
#include "main_header.h"

#define HISTORY_ATOM_RAND_N_RET_VAL		0
#define HISTORY_ATOM_LEFT_CLICK			1
#define HISTORY_ATOM_RIGHT_CLICK		2
#define HISTORY_ATOM_DOUBLE_CLICK		3

class history {
	struct atom {
		int type;
		v2i point;
		atom (int type_ = 0, int x = 0, int y = 0) : type (type_), point (x, y) {}
	};
	vec <atom> data;
public:
	void clear () {data.clear ();};
	bool empty () {return data.empty ();}
	void store ();
	void load ();
	void push_rand_n (int rnd);
	void push_right_click (v2i pos);
	void push_left_click (v2i pos);
	void push_double_click (v2i pos);

	int get_rand_n (int n);
	bool is_next_click_right ();
	bool is_next_click_left ();
	bool is_next_click_double ();
	v2i get_next_point ();
};