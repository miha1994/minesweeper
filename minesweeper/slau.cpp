#include "slau.h"

/*
class slau_1 {
public:
	int h;
	int w_A_with_b;
	cell_ar <r_n> A_b;
	std::vector <v2i> link;
	explicit slau_1 (int h_, int w_of_A_without_b) : A_b (h, w_of_A_without_b + 1) {
		h = h_;
		w_A_with_b = w_of_A_without_b + 1;
		A_b.init (r_n (0,1));
	}
	void solve (slau_2 *);
};
*/

#define me (copy.A_b)

#define swap(T,a,b) {T tmp = (a);(a) = (b);(b) = tmp;}

slau_2 slau_1::solve () const {
	slau_1 copy (*this);
	int last = h;
	FOR (i, last) {  // прямой ход
		int i_non_zero = -1;
		do {
			for (int z = i; z < w_A_with_b - 1; ++z) {
				if (me[i][z] != 0) {
					i_non_zero = z;
					break;
				}
			}
			if (i_non_zero >= 0 || --last == i) {
				break;
			}
			FOR (z, w_A_with_b) {
				swap (int, me[i][z],me[last][z]);
			}
		} while (1);
		if (i == last) {
			break;
		}
		if (i_non_zero != i) {
			for (int j = 0; j < last; ++j) {
				swap (int, me[j][i], me[j][i_non_zero]);
			}
			swap (v2i, copy.link[i], copy.link[i_non_zero]);
		}
		int first_non_zero_el = me[i][i];
		if (first_non_zero_el != 1) {
			for (int z = i + 1; z < w_A_with_b; ++z) {
				me[i][z] = me[i][z] / first_non_zero_el;
			}
		}
		for (int j = i+1; j < last; ++j) {
			if (me[j][i] != 0) {
				int mn = me[j][i];
				for (int z = i; z < w_A_with_b; ++z) {
					me[j][z] = me[j][z] - mn * me[i][z];
				}
			}
		}
	}
	for (int i = last-1; i; --i) { // обратный ход
		FOR (j, i) {
			if (me[j][i] != 0) {
				int mn = me[j][i];
				for (int z = i; z < w_A_with_b; ++z) {
					me[j][z] = me[j][z] - mn * me[i][z];
				}
			}
		}
	}
	slau_2 rv (last, w_A_with_b - 1 - last);
	rv.link_h.assign (copy.link.begin (), copy.link.begin () + last);
	rv.link_w.assign (copy.link.begin () + last, copy.link.end ());

	FOR (i, rv.h) {
		FOR (j, rv.w_A_with_b) {
			if (j == rv.w_A_with_b - 1) {
				rv.A_b[i][j] = me[i][j + last];
			} else {
				rv.A_b[i][j] = -me[i][j + last];
			}
		}
	}
	return rv;
}

#define me (this->A_b)

void slau_2::solve () {
	int esp = 0;
	int w_A_without_b = w_A_with_b - 1;
	slau_elem_info sei;
	bool *row_used = new bool [h];
	bool *column_used = new bool [w_A_without_b];
	int *min_a = new int[h], *max_a = new int[h], mnx;
	int *stack = new int [w_A_without_b];
	int index_of_mnx;
	memset (row_used, 0, sizeof (bool) * h);
	memset (column_used, 0, sizeof (bool) * w_A_without_b);
	inf_h.assign (h, sei);
	inf_w.assign (w_A_without_b, sei);

	while (1) {
		index_of_mnx = -1;
		FOR (i, h) {
			if (!row_used[i]) {
				min_a[i] = me[i][w_A_without_b], max_a[i] = me[i][w_A_without_b];
				FOR (j, w_A_without_b) {
					if (column_used[j]) {
						if (GET_F_STATE (inf_w[j], SLAU_ELEM_INFO_FLAG_IS_EQUAL_TO_ONE)) {
							min_a[i] += me[i][j];
							max_a[i] += me[i][j];
						}
					} else {
						if (me[i][j] > 0) {
							max_a[i] += me[i][j];
						} else {
							min_a[i] += me[i][j];
						}
					}
				}
				int cur = (1 - min_a[i]) * (max_a[i]);
				if (index_of_mnx < 0 || cur < mnx) {
					index_of_mnx = i;
					mnx = cur;
				}
			}
		}
		if (mnx == 0 && index_of_mnx >= 0) {
			row_used[index_of_mnx] = true;
			FOR (j, w_A_without_b) {
				if (!column_used[j] && me[index_of_mnx][j] != 0) {
					stack[esp] = j;
					column_used[j] = true;
					SET_F_STATE (inf_w[j], SLAU_ELEM_INFO_FLAG_CONST, true);
					SET_F_STATE (inf_w[j], SLAU_ELEM_INFO_FLAG_IS_EQUAL_TO_ONE, bool (max_a[index_of_mnx] == 0) ^ bool (me[index_of_mnx][j] < 0));			
					++esp;
				}
			}
		}
		if (mnx > 0 && index_of_mnx >= 0) {
			FOR (j, w_A_without_b) {
				if (!column_used[j] && me[index_of_mnx][j] != 0) {
					stack[esp] = j;
					column_used[j] = true;
					SET_F_STATE (inf_w[j], SLAU_ELEM_INFO_FLAG_CONST, false);
					SET_F_STATE (inf_w[j], SLAU_ELEM_INFO_FLAG_IS_EQUAL_TO_ONE, false);
					++esp;
					break;
				}
			}
		}
		if (esp == w_A_without_b) {
			FOR (j, w_A_without_b) {
				SET_F_STATE (inf_w[j], GET_F_STATE (inf_w[j], SLAU_ELEM_INFO_FLAG_IS_EQUAL_TO_ONE) ? SLAU_ELEM_INFO_FLAG_CAN_BE_ONE : SLAU_ELEM_INFO_FLAG_CAN_BE_ZERO, true);
			}
			int res = 0;
			FOR (i, h) {
				res = 0;
				FOR (j, w_A_with_b) {
					if (j == w_A_without_b || GET_F_STATE (inf_w[j], SLAU_ELEM_INFO_FLAG_IS_EQUAL_TO_ONE)) {
						res += me[i][j];
					}
				}
				
				SET_F_STATE (inf_h[i], (res == 1) ? SLAU_ELEM_INFO_FLAG_CAN_BE_ONE : SLAU_ELEM_INFO_FLAG_CAN_BE_ZERO, true);
			}
		}
		if (mnx < 0 || esp == w_A_without_b || index_of_mnx < 0) {
			//--esp;
			for (--esp; ; --esp) {
				if (esp < 0) {
					delete []row_used;
					delete []column_used;
					delete []min_a;
					delete []max_a;
					delete []stack;
					return;
				}
				if (GET_F_STATE (inf_w[stack[esp]], SLAU_ELEM_INFO_FLAG_CONST) ) {
					SET_F_STATE (inf_w[stack[esp]], SLAU_ELEM_INFO_FLAG_CONST, false);
					column_used[stack[esp]] = false;
					FOR (i, h) {
						if (me[i][stack[esp]] != 0) {
							row_used [i] = false;
						}
					}
					continue;
				}
				SET_F_STATE (inf_w[stack[esp]], SLAU_ELEM_INFO_FLAG_IS_EQUAL_TO_ONE, true);
				SET_F_STATE (inf_w[stack[esp]], SLAU_ELEM_INFO_FLAG_CONST, true);
				++esp;
				break;
			}
			continue;
		}
	}
}