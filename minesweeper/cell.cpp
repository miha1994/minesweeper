#include "main_header.h"
#include "cell.h"
#include "quad.h"
#include "o_field.h"

cell_ar::cell_ar (int h, int w) {
	f = new cell [(w+2) * (h+2)];
	W = w;
	H = h;
	tc = new tmp_cell [h+2];
	FOR (i, h+2) {
		tc[i].f = f + i*(w+2);
	}
}

/*
#define CELL_FLAGS_MINE			0x1
#define CELL_FLAGS_CLOSED		0x2
#define CELL_FLAGS_MARK			0x4
*/

void field_cells_upd (char *o, float dt) {
	O_DECL (field, fld);
	fld->cells.clear ();
	fld->mines.clear ();
	fld->digits.clear ();
	quad q;
	quad_set_tex_rect (q, sf::IntRect (0,0,30,30));
	FOR (i, fld->gp.height) {
		FOR (j, fld->gp.width) {
			quad_set_pos (q, v2f(7 + j*32, 44 + i*32), v2f(0,0));
			if (fld->a[i][j].flags & CELL_FLAGS_CLOSED) {
				if (fld->a[i][j].flags & CELL_FLAGS_MINE) {
					quad_set_color (q, my_clr_s[color_theme].mine);
					quad_v_a (q, &fld->mines);
				}
				quad_set_color (q, my_clr_s[color_theme].unknown);
				quad_v_a (q, &fld->cells);
			}
			else {
				quad_set_color (q, my_clr_s[color_theme].safe);
				quad_v_a (q, &fld->cells);
			}
		}
	}
	for (std::list<force_cell>::iterator p = (fld->fc_l).begin(); p != (fld->fc_l).end(); ) {
		if (p->upd (&fld->cells, dt, q) ){
			p = fld->fc_l.erase (p);
		} else {
			++p;
		}
	}
	quad_set_tex_rect (q, sf::IntRect (0, 0, MY_WIND_WIDTH - 14, 30));
	quad_set_pos (q, v2f(7, 7), v2f(0,0));
	quad_set_color (q, my_clr_s[color_theme].unknown);
	quad_v_a (q, &fld->cells);
}

bool force_cell::upd (sf::VertexArray *va, float dt, quad q) {
	time += dt;
	quad_set_pos (q, v2f(7 + pos.x*32, 44 + pos.y*32), v2f(0,0));
	if (time > duration) {
		return true;
	}
	quad_set_color (q, (fin * (time / duration)) + (start * ((duration - time) / duration)));
	quad_v_a (q, va);
	return false;
}