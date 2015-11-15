#pragma once
#include "main_header.h"
#include "some_structures.h"
#include "quad.h"

#define CELL_FLAGS_MINE			0x1
#define CELL_FLAGS_CLOSED		0x2
#define CELL_FLAGS_MARK			0x4

class cell {
public:
	int flags;
	int val;
};

class tmp_cell {
public:
	cell *f;
	cell& operator[](size_t idx) const { return *(f + idx + 1); };
};

class cell_ar {
public:
	cell *f;
	tmp_cell *tc;
	int H, W;
	explicit cell_ar (int h, int w);
	~cell_ar () {delete []f; delete []tc;}
	const tmp_cell& operator[](size_t idx) const { return *(tc + idx + 1); };
	cell &operator[](v2i p) const { return *((tc + p.y + 1)->f + p.x + 1); };
};

class force_cell {
public:
	v2i pos;
	CLR start;
	CLR fin;
	float time;
	float duration;
	force_cell (v2i position, CLR c1, CLR c2, float dur) : pos (position), start(c1), fin (c2), time (0), duration (dur) {}
	bool upd (sf::VertexArray *va, float dt, quad q);
};

void field_cells_upd (char *o, float dt);