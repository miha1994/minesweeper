#include "o_button.h"
#include "o_control.h"
#include "space.h"
#include "save.h"
#include "quad.h"

O_UPDATE (button_update) {
	CNTRL ("button_update");
	O_DECL (button, btn);

	v2i m;
	get_mouse_pos (m);
	forvector (p, end, sq_button, btn->play_buttons) {
		p->update (dt, v2f(m));
	}
	if (btn->add.update (dt, v2f(m))) {
		bool add = true;
		if (btn->save.Num_of_templates > 5) {
			add = false;
		} else {
			forvector (p, end, game_parameters, btn->save.templates) {
				if (p->width == btn->hwm[1].value && 
					p->height == btn->hwm[0].value &&
					p->mines == btn->hwm[2].value) {
						add = false;
						break;
				}
			}
		}
		if (add) {
			btn->save.Num_of_templates++;
			game_parameters gp;
			gp.width = btn->hwm[1].value;
			gp.height = btn->hwm[0].value;
			gp.mines = btn->hwm[2].value;
			btn->save.templates.push_back (gp);
			make_table (btn);
			set_save_status (btn->save);
		}
	}

    btn->crt.update ();
	bool need_upd = false;
	int num_of_upd;
	if (mouse_left_pressed) {	
		FOR (i, 3) {
			if (sf::IntRect (btn->x[i], btn->y[1], btn->x[i+1] - btn->x[i] - 2, btn->y[1+1] - btn->y[1] - 2).contains (m)) {
				btn->hwm[i].active = true;
				btn->hwm[i].time = 1;
				need_upd = true;
				num_of_upd = i;
			} else if (btn->hwm[i].active) {
				btn->hwm[i].active = false;
				if (!btn->hwm[i].value || (i < 2 && btn->hwm[i].value < 2)) {
					btn->hwm[i].upd (10);
				}
				btn->hwm[2].max_val = Min (999, btn->hwm[0].value * btn->hwm[1].value - 1);
				if (btn->hwm[2].max_val < 1) {
					btn->hwm[2].max_val = 10;
				}
				btn->hwm[2].upd(btn->hwm[2].value);
			}
		}
	}
	if (need_upd) {
		btn->hwm[num_of_upd].upd (0);
	}
	if (kb::isKeyPressed (kb::Return) ){
		FOR (i, 3) {
			if (btn->hwm[i].active) {
				btn->hwm[i].active = false;
				if (!btn->hwm[i].value || (i < 2 && btn->hwm[i].value < 2)) {
					btn->hwm[i].upd (10);
				}
				btn->hwm[2].max_val = Min (999, btn->hwm[0].value * btn->hwm[1].value - 1);
				if (btn->hwm[2].max_val < 1) {
					btn->hwm[2].max_val = 10;
				}
				btn->hwm[2].upd(btn->hwm[2].value);
			}
		}
	}
	FOR (i, 3) {
		if (btn->hwm[i].active) {
			if ((btn->hwm[i].time -= dt*1.5) < 0) {
				btn->hwm[i].time += 1.7;
			}
			FOR (j, 10) {
				if (kb::isKeyPressed (kb::Key (int(kb::Num0) + j))) {
					btn->hwm[i].upd (btn->hwm[i].value * 10 + j);
					while (kb::isKeyPressed (kb::Key (int(kb::Num0) + j))) {
					}
				}
			}
			if (kb::isKeyPressed (kb::BackSpace)) {
				btn->hwm[i].upd (btn->hwm[i].value / 10);
				while (kb::isKeyPressed (kb::BackSpace)) {
				}
			}
		}
	}
	btn->hwm[2].max_val = Min (999, btn->hwm[0].value * btn->hwm[1].value - 1);
	return false;
}