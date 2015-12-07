#include "main_header.h"
#include "cell.h"
#include "quad.h"
#include "o_field.h"

/*
#define CELL_FLAGS_MINE			0x1
#define CELL_FLAGS_CLOSED		0x2
#define CELL_FLAGS_MARK			0x4
*/

void field_cells_upd (char *o, float dt, v2i double_point) {
	O_DECL (field, fld);
	fld->cells.clear ();
	fld->forced_cells.clear ();
	fld->mines.clear ();
	fld->digits.clear ();
	fld->flags.clear ();
	fld->crs.clear ();
	quad q;
	quad_set_tex_rect (q, sf::IntRect (0,0,30,30));
	FOR_2D (v, WWW, HHH) {
		quad_set_pos (q, v2f(v2i(7, 44) + v*32), v2f(0,0));
		MK_C (c, v);
		if (c->flags & CELL_FLAGS_CLOSED) {
			if (fld->game_over && (c->flags & CELL_FLAGS_MINE) && !(c->flags & CELL_FLAGS_MARK)) {
				quad_set_color (q, my_clr_s[color_theme].mine);
				quad_v_a (q, &fld->mines);
			}
			if (c->flags & CELL_FLAGS_MARK) {
				if (!fld->game_over || (c->flags & CELL_FLAGS_MINE)) {
					quad_set_color (q, my_clr_s[color_theme].flag);
					quad_v_a (q, &fld->flags);
				} else {
					quad_set_color (q, my_clr_s[color_theme].mine);
					quad_v_a (q, &fld->mines);
					quad_set_color (q, CLR::Black);
					quad_v_a (q, &fld->crs);
				}
			}
			if (kb::isKeyPressed (kb::S)) {
				if (c->flags & CELL_FLAGS_MINE) {
					quad_set_color (q, my_clr_s[color_theme].font);
					quad_v_a (q, &fld->mines);
				}
				if (c->flags & CELL_P_SAFE) {
					quad_set_color (q, CLR::Red);
					quad_v_a (q, &fld->crs);
				}
				quad_set_color (q, (c->flags & CELL_Q) ? CLR::Blue : my_clr_s[color_theme].unknown);
			} else {
				if (length (v2f(double_point - v)) > 1.7) {
					quad_set_color (q, my_clr_s[color_theme].unknown);
				} else {
					if (c->flags & CELL_FLAGS_MARK) {
						quad_set_color (q, my_clr_s[color_theme].unknown);
					} else {
						quad_set_color (q, my_clr_s[color_theme].safe);
					}
				}
			}
			quad_v_a (q, &fld->cells);
		}
		else {
			quad_set_color (q, my_clr_s[color_theme].safe);
			quad_v_a (q, &fld->cells);
		}
		if (! (c->flags & CELL_FLAGS_CLOSED) && c->val) {
			quad_set_color (q, dig_colors[c->val]); //my_clr_s[color_theme].font);
			quad_set_tex_rect (q, sf::Rect <int> (c->val * 30, 0, 30, 30));
			quad_v_a (q, &fld->digits);
			quad_set_tex_rect (q, sf::IntRect (0,0,30,30));
		}
	}
	for (std::list<force_cell>::iterator p = (fld->fc_l).begin(); p != (fld->fc_l).end(); ) {
		if (p->upd (&fld->forced_cells, dt, q) ){
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
	if ((wait_time -= dt) < 0) {
		time += -wait_time;
		wait_time = 0;
		quad_set_pos (q, v2f(7 + pos.x*32, 44 + pos.y*32), v2f(0,0));
		if (time > duration) {
			return true;
		}
		quad_set_color (q, (CLR_ADD (     fin * (time / duration),     (start * ((duration - time) / duration))      )));
		quad_v_a (q, va);
		return false;
	} else {
		return false;
	}
}