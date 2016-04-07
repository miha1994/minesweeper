#pragma once
#include "main_header.h"
#include "some_structures.h"
#include "quad.h"

#define CELL_FLAGS_MINE			0x1
#define CELL_FLAGS_CLOSED		0x2
#define CELL_FLAGS_MARK			0x4
#define CELL_FLAGS_Q			0x8
#define CELL_P_MINE             0x10
#define CELL_P_SAFE             0x20
#define CELL_Q                  0x40

class cell {
public:
	int flags;
	int val;
};

template <class T>
class tmp_cell {
public:
	T *f;
	T& operator[](size_t idx) const { return *(f + idx + 1); };
};

template <class T>
class cell_ar {
public:
	T *f;
	tmp_cell <T> *tc;
	int H, W;
	explicit cell_ar (int h, int w);
	~cell_ar () {delete []f; delete []tc;}
	const tmp_cell <T>& operator[](size_t idx) const { return *(tc + idx + 1); };
	T &operator[](v2i p) const { return *((tc + p.y + 1)->f + p.x + 1); };
	void init (T init_val) { FOR (i, (W+2) * (H+2)) f[i] = init_val; }
	cell_ar &operator = (const cell_ar&);
	cell_ar (const cell_ar &);
};
template <class T>
cell_ar <T>::cell_ar (int h, int w) {
	f = new T [(w+2) * (h+2)];
	W = w;
	H = h;
	tc = new tmp_cell <T> [h+2];
	FOR (i, h+2) {
		tc[i].f = f + i*(w+2);
	}
}

template <class T>
cell_ar<T> & cell_ar<T>::operator = (const cell_ar<T>& ca) {
	if (H != ca.H || W != ca.W) {
		delete []f;
		delete []tc;
		W = ca.W;
		H = ca.H;
		f = new T [(W+2) * (H+2)];
		tc = new tmp_cell <T> [H+2];
		FOR (i, H+2) {
			tc[i].f = f + i*(W+2);
		}
	}
	memcpy (f, ca.f, sizeof (T) * (W+2) * (H+2));
    return *this;
}

template <class T>
cell_ar<T>::cell_ar (const cell_ar<T> &s) {
	W = s.W;
	H = s.H;
	f = new T [(W+2) * (H+2)];
	tc = new tmp_cell <T> [H+2];
	FOR (i, H+2) {
		tc[i].f = f + i*(W+2);
	}
	memcpy (f, s.f, sizeof (T) * (W+2) * (H+2));
}

class field;

class force_cell {
public:
	v2i pos;
	CLR start;
	CLR fin;
	float time;
	float duration;
	float wait_time;
	force_cell (v2i position, CLR c1, CLR c2, float dur, float wait = 0.0f) : pos (position), start(c1), fin (c2), time (0), duration (dur), wait_time(wait) {}
	bool upd (sf::VertexArray *va, float dt, quad q, int ptx, field *fld);
};

void field_cells_upd (char *o, float dt, v2i double_point);