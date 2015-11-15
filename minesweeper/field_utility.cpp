#include "field_utility.h"

void field_make (field *fld, std::list <v2i> *free_cells, int mines) {
	For (mines) {
		int nxt = rand_n (free_cells->size ());
		v2i mine_pos;
		forlist (p, end, v2i, *free_cells) {
			if (!(nxt--)) {
				mine_pos = *p;
				free_cells->erase (p);
				break;
			}
		}
		fld->a[mine_pos].flags |= CELL_FLAGS_MINE;
	}
}