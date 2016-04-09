#include "main_header.h"
#include "cell.h"
#include "quad.h"
#include "o_field.h"

/*
#define CELL_FLAGS_MINE			0x1
#define CELL_FLAGS_CLOSED		0x2
#define CELL_FLAGS_MARK			0x4
*/

const float sv_ = 6, sv_sec_ = 3;

void field_cells_upd (char *o, float dt, v2i double_point) {
	O_DECL (field, fld);
	fld->expl.update (dt);
	fld->cells.clear ();
	fld->forced_cells.clear ();
	if (!fld->win ()) {
		fld->mines.clear ();
	}
	fld->digits.clear ();
	fld->bg_cells.clear ();
	fld->flags.clear ();
	fld->crs.clear ();
	fld->free.clear ();
	fld->q_array.clear ();

	quad q;
	quad_set_tex_rect (q, SFRECTcv (int, 0,0,fld->pix > 48 ? v2f(96,96) : v2f (48, 48)));
    CLR safe = my_clr_s[color_theme].safe;
	if (fld->win ()) {
		if (fld->wm.state) {
			safe.a = 100;
		} else {
			safe.a = (255 - 100) * (1 - fld->win_time) + 100;
		}
		safe.a = 100;
	}
	CLR unknown;
	/*
	if (fld->win && fld->win_time < sv_sec_) {
		float mul = (cos (fld->win_time*sv_) + 1) / 2.0;
		unknown = CLR_ADD (my_clr_s[color_theme].unknown * (mul), CLR (0, 180, 0) * (1-mul));
	} else {
		unknown = my_clr_s[color_theme].unknown;
	}
	*/
	unknown = my_clr_s[color_theme].unknown;
	if (fld->win ()) {
		unknown = my_clr_s[color_theme].safe;
		if (fld->wm.state) {
			unknown.a = 100;
		} else {
			unknown.a = (255 - 100) * (1 - fld->win_time) + 100;
		}
		unknown.a = 100;
	}
    const float rt_max = 0.9;
    if (fld->reallocation_time < rt_max) {
        float sn = sin ((fld->reallocation_time / rt_max) * 3.14);
        unknown = CLR_ADD(unknown * (1 - sn), CLR (200,100,100,255) * sn);
    }

	CLR gray(150,0,0,105);
	CLR Mine = my_clr_s[color_theme].mine;
	if (fld->game_over) {
		if (fld->win_time < 3) {
			float sn = fld->win_time/3;
			Mine = CLR_ADD(Mine * (1 - sn), gray * sn);
		} else {
			Mine = gray;
		}
	}
	CLR Free (CLR::White);
	if (fld->game_over) {
		if (fld->win_time < 3) {
			float sn = fld->win_time/3;
			Free = CLR_ADD(CLR(255,255,255,0) * (1 - sn), Free * sn);
		}
	}
	FOR_2D (v, WWW, HHH) {
		quad_set_pos (q, v2f (fld->shift, fld->vshift) + v2f(v2i(6, 43) + v*32) * PIX, v2f(fld->pix,fld->pix),false);
		MK_C (c, v);
		if (c->flags & CELL_FLAGS_CLOSED) {
			if (fld->game_over && (c->flags & CELL_FLAGS_MINE) && !(c->flags & CELL_FLAGS_MARK)) {
				if (fld->game_over) {
					if (v == fld->fail_choice) {
						quad_set_color (q, CLR::Black);
					} else {
						if (!(c->flags & CELL_P_MINE)) {
							quad_set_color (q, Mine);
						} else {
							quad_set_color (q, my_clr_s[color_theme].mine);
						}
					}
				} else {
					quad_set_color (q, my_clr_s[color_theme].mine);
				}
				quad_v_a (q, &fld->mines);
			}
			if (c->flags & CELL_FLAGS_MARK) {
				if (!fld->game_over || (c->flags & CELL_FLAGS_MINE)) {
					if (!fld->win ()) {
						quad_set_color (q, my_clr_s[color_theme].flag);
						quad_v_a (q, &fld->flags);
					}
				} else {
					quad_set_color (q, my_clr_s[color_theme].mine);
					quad_v_a (q, &fld->mines);
					quad_set_color (q, my_clr_s[color_theme].mine);
					quad_v_a (q, &fld->crs);
				}
			}
			if (c->flags & CELL_FLAGS_Q) {
				if (!fld->game_over && !fld->win ()) {
					quad_set_color (q, CLR::Black);
					quad_v_a (q, &fld->q_array);
				}
			}
			if (false) {//kb::isKeyPressed (kb::S)) {
				if (c->flags & CELL_FLAGS_MINE) {
					quad_set_color (q, my_clr_s[color_theme].mine);
					quad_v_a (q, &fld->mines);
				}
				if (c->flags & CELL_P_SAFE) {
					quad_set_color (q, CLR::Red);
					quad_v_a (q, &fld->crs);
				}
				quad_set_color (q, (c->flags & CELL_Q) ? CLR::Blue : unknown);
			} else {
				if (length (v2f(double_point - v)) > 1.7 ) {
					if (fld->game_over && v == fld->fail_choice) {
						quad_set_color (q, my_clr_s[color_theme].mine);
					} else {
						if (c->flags & CELL_P_SAFE && fld->game_over) {
							quad_set_color (q, Free);
							quad_v_a (q, &fld->free);
						}
						quad_set_color (q, unknown);
					}
				} else {
					if (c->flags & CELL_FLAGS_MARK || fld->game_over || fld->win ()) {
						quad_set_color (q, unknown);
					} else {
						quad_set_color (q, my_clr_s[color_theme].safe);
					}
				}
			}
			quad_v_a (q, fld->win () ? &fld->bg_cells : &fld->cells);
		}
		else {
			quad_set_color (q, safe);
			quad_v_a (q, fld->win () ? &fld->bg_cells : &fld->cells);
		}
		if (! (c->flags & CELL_FLAGS_CLOSED) && c->val && (!fld->win ())) {
			quad_set_color (q, dig_colors[c->val]); // my_clr_s[color_theme].font);
			quad_set_tex_rect (q, SFRECTcv (int, c->val * (fld->pix > 48 ? 96 : 48), 0, fld->pix > 48 ? v2f(96,96) : v2f (48, 48)));
			quad_v_a (q, &fld->digits);
			quad_set_tex_rect (q, SFRECTcv (int, 0, 0, fld->pix > 48 ? v2f(96,96) : v2f (48, 48)));
		}
	}
	for (std::list<force_cell>::iterator p = (fld->fc_l).begin(); p != (fld->fc_l).end(); ) {
		if (p->upd (&fld->forced_cells, dt, q, fld->pix, fld) ){
			p = fld->fc_l.erase (p);
		} else {
			++p;
		}
	}
	quad_set_tex_rect (q, sf::IntRect (4, 4, 1, 1));
	quad_set_pos (q, v2f(fld->shift ,0), v2f(MY_WIND_WIDTH, 37) * PIX, 0);
	quad_set_color (q, my_clr_s[color_theme].unknown);
	quad_v_a (q, &fld->cells);
}

bool force_cell::upd (sf::VertexArray *va, float dt, quad q, int pix, field *fld) {
	if ((wait_time -= dt) < 0) {
		time += -wait_time;
		wait_time = 0;
		quad_set_pos (q, v2f (fld->shift, fld->vshift) + v2f(6 + pos.x*32, 43 + pos.y*32) * float(PIX), v2f(pix, pix), 0);
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