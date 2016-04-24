#include "o_field.h"
#include "o_control.h"
#include "space.h"
#include "save.h"
#include "quad.h"
#include "field_utility.h"
#include "input.h"
#include "animation.h"

std::string sm_file_names [4] = {
	"none",
	"danger",
	"win",
	"game_over"
};

O_LOAD (field_load);

#define DF(smth)    smth (fld.smth)
#define DF1(q1,q2,q3,q4,q5)    DF(q1), DF(q2), DF(q3), DF(q4), DF(q5)
field::field (field& fld) : DF(state),
	DF (mkr),
    DF (sm),
	DF (wm),
	DF (mm),
    DF (a),
    DF (gp),
    DF (bg),
    DF (fc_l),
    DF (new_digits),
    DF (menu),
    DF (cells),
    DF (forced_cells),
    DF (flags),
    DF (mines),
    DF (digits),
    DF (crs),
    DF (mines_left),
    DF (sec),
    DF1 (time, pix, shift, vshift, win_time),
    DF1 (r_wind_h, r_wind_w, intro_num, inside_w, empty),
    DF1 (game_over, z_win_, wait_all_release, no_moves, crt),
    commit (&fld) {}
#undef DF1
#undef DF


#define DF(s)   s = commit->s;
#define DF1(q1,q2,q3,q4,q5)    DF(q1); DF(q2); DF(q3); DF(q4); DF(q5)
void field::checkout () {
    DF (state);
	DF (mkr);
    DF (sm);
	DF (wm);
	DF (mm);
    DF (a);
    DF (gp);
    DF (bg);
    DF (fc_l);
    DF (new_digits);
    DF (menu);
    DF (cells);
    DF (forced_cells);
    DF (flags);
    DF (mines);
    DF (digits);
    DF (crs);
    DF (mines_left);
    DF (sec);
    DF1 (time, pix, shift, vshift, win_time);
    DF1 (r_wind_h, r_wind_w, intro_num, inside_w, empty);
    DF1 (game_over, z_win_, wait_all_release, no_moves, crt);
}
#undef DF1
#undef DF

O_UPDATE (field_update);

O_RENDER (field_render) {
	O_DECL (field, fld);
	window.draw (fld->bg);
	if (fld->win ()) {
		window.draw (fld->mines, &fld->mine_text[fld->pix > 48]);
	}
	window.draw (fld->bg_cells, &fld->cell_text[PX_1]);
	if (fld->show_results.is_dead == false && fld->show_results.is_available_to_be_pressed) {
		fld->show_results.draw (&window, fld->mm.m_time > 2 ? 255 : 255.0 / 2 * fld->mm.m_time);
	}
	window.draw (fld->cells, &fld->cell_text[PX_1]);
	window.draw (fld->digits, &fld->digits_text[PX_1]);
	window.draw (fld->forced_cells, &fld->cell_text[PX_1]);
	if (!fld->win ()) {
		window.draw (fld->mines, &fld->mine_text[PX_1]);
	}
	
	window.draw (fld->flags, &fld->flag_text[PX_1]);
	window.draw (fld->q_array, &fld->q_text[PX_1]);
	window.draw (fld->free, &fld->free_text[PX_1]);
	window.draw (fld->crs, &fld->crs_text[PX_1]);

	fld->menu.draw (&window);
	fld->sm.spr[fld->sm.state][PX_1].draw (&window);
    fld->hint.draw (&window);
	if (fld->b_switch_mine_moving_ability || fld->b_switch_safe_opening_ability) {
		window.draw (fld->reallocation_count.txt);
	}
	
	window.draw (fld->sec);
	window.draw (fld->mines_left);
	if (fld->win ()) {
		fld->wm.spr[PX_1].draw (&window);
	}
	if (fld->game_over) {
		fld->expl.draw (&window);
	}
	//fld->crt.render ();
}

O_DEL (field_del) {
	O_DECL (field, fld);
	delete fld;
}

std_obj_functions field_f (field_load, field_update, field_render, field_del);