#include "o_button.h"
#include "o_control.h"
#include "space.h"
#include "save.h"
#include "quad.h"
#include "input.h"

O_UPDATE (button_update) {
	CNTRL ("button_update");
	O_DECL (button, btn);

	v2i m;
	get_mouse_pos (m);
    int i = 0;
	forvector (p, end, sq_button, btn->play_buttons) {
		if (p->update (dt, v2f(m))) {
			if (i == 0) {
				global_game_parameters.height = btn->hwm[0].value;
				global_game_parameters.width = btn->hwm[1].value;
				global_game_parameters.mines = btn->hwm[2].value;
			} else {
				global_game_parameters = btn->save.templates[i-1];
			}

            MY_WIND_WIDTH = 12 + global_game_parameters.width * 32;//f_modes[0].width;
			MY_WIND_HEIGHT = 49 + global_game_parameters.height * 32;//f_modes[0].height;

            window.close ();
            window.create (sf::VideoMode (MY_WIND_WIDTH, MY_WIND_HEIGHT), "Minesweeper", sf::Style::Default);
	        window.setVerticalSyncEnabled (true);
	        window.setFramerateLimit (60);

			control *mc = (control *)main_control;
			mc->todo.push_back (instruction ("delete", "MENU"));
			mc->todo.push_back (instruction ("load", "L0"));
			mc->todo.push_back (instruction ("active", "L0"));
        }
        ++i;
	}
    i = 0;
    bool del = false;
    forvector (p, end, sq_button, btn->del_buttons) {
		if (p->update (dt, v2f(m)) && !del) {
            del = true;
            btn->save.Num_of_templates--;
            btn->save.templates.erase (btn->save.templates.begin () + i + 3);
			make_table (btn);
			set_save_status (btn->save);
        }
        ++i;
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
	if (input.mbutton[MOUSE_LEFT].pressed_now) {
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
                btn->hwm[i].upd_min ();
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