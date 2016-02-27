#pragma once
#include "cell.h"

#define SLAU_ELEM_INFO_FLAG_CAN_BE_ZERO			0x1
#define SLAU_ELEM_INFO_FLAG_CAN_BE_ONE			0x2
#define SLAU_ELEM_INFO_FLAG_CONST				0x4
#define SLAU_ELEM_INFO_FLAG_IS_EQUAL_TO_ONE		0x8

//#define SLAU_ELEM_INFO_FLAG_CAN_BE_ZERO		0x8

#define GET_F_STATE(slau_ei, flag)	((slau_ei.state) & (flag))
#define SET_F_STATE(slau_ei, flag, value)	{if (value) { \
		((slau_ei).state) |= (flag);\
	} else { \
		((slau_ei).state) &= ~(flag);\
	}}
		

class slau_elem_info {
public:
	int state;
	slau_elem_info () : state (0) {}
};

class slau_2 {
public:
	int h;
	int w_A_with_b;
	cell_ar <r_n> A_b;
	std::vector <v2i> link_h;
	std::vector <v2i> link_w;
	std::vector <slau_elem_info> inf_h;
	std::vector <slau_elem_info> inf_w;
	explicit slau_2 (int h_, int w_of_A_without_b) : A_b (h_, w_of_A_without_b + 1) {
		h = h_;
		w_A_with_b = w_of_A_without_b + 1;
		A_b.init (r_n (0,1));
	}
	void solve ();
};

class slau_1 {
public:
	int h;
	int w_A_with_b;
	cell_ar <r_n> A_b;
	std::vector <v2i> link;
	explicit slau_1 (int h_, int w_of_A_without_b) : A_b (h_, w_of_A_without_b + 1) {
		h = h_;
		w_A_with_b = w_of_A_without_b + 1;
		A_b.init (r_n (0,1));
	}
	slau_2 solve () const;
};
