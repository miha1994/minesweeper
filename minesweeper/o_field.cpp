#include "o_field.h"
#include "o_control.h"
#include "space.h"
#include "save.h"
#include "quad.h"
#include "field_utility.h"
#include "input.h"

std::string sm_file_names [4] = {
	"none.png",
	"danger.png",
	"win.png",
	"game_over.png"
};

void field_reset (field *fld) {
	for (int i = -1; i <= fld->gp.height; ++i) {
		for (int j = -1; j <= fld->gp.width; ++j) {
			fld->a[i][j].flags = CELL_FLAGS_CLOSED;
			fld->a[i][j].val = 0;
		}
	}
	fld->empty = true;
	fld->game_over = false;
	fld->win = false;
	fld->time = 0.0f;
	fld->no_moves = false;
}

O_LOAD (field_load) {
	field *fld = new field (global_game_parameters.height, global_game_parameters.width);
	fld->gp = global_game_parameters;

	fld->font.loadFromFile ("assets/fonts/cg.ttf");

	fld->sec.setFont (fld->font);
	fld->sec.setColor (my_clr_s[color_theme].font);
	fld->sec.setString ("999");
	fld->sec.setCharacterSize (20);

	fld->mines_left.setFont (fld->font);
	fld->mines_left.setColor (my_clr_s[color_theme].font);
	fld->mines_left.setString ("999");
	fld->mines_left.setCharacterSize (20);
	fld->sec.setPosition(928,8);
	fld->mines_left.setPosition(63,9);

	fld->bg.setPosition (0,0);
	fld->bg.setSize (v2f(MY_WIND_WIDTH, MY_WIND_HEIGHT));
	fld->bg.setFillColor (my_clr_s[color_theme].bg);

	fld->cells.setPrimitiveType (sf::Quads);
	fld->forced_cells.setPrimitiveType (sf::Quads);
	fld->mines.setPrimitiveType (sf::Quads);
	fld->digits.setPrimitiveType (sf::Quads);
	fld->flags.setPrimitiveType (sf::Quads);
	fld->crs.setPrimitiveType (sf::Quads);
	
	fld->digits_text.loadFromFile ("assets/textures/digits.png");
	fld->digits_text.setSmooth (false);
	fld->mine_text.loadFromFile ("assets/textures/mine1.png");
	fld->mine_text.setSmooth (false);
	fld->flag_text.loadFromFile ("assets/textures/flag.png");
	fld->flag_text.setSmooth (false);
	fld->crs_text.loadFromFile ("assets/textures/cross.png");
	fld->crs_text.setSmooth (false);

	fld->menu_na.init ("assets/textures/menu_na.png", 50, 30);
	fld->menu_a.init ("assets/textures/menu_a.png", 50, 30);
	fld->menu.alpha = 0;
	fld->menu.pos = v2i (7, 7);
	fld->menu.act = &fld->menu_a;
	fld->menu.spr = &fld->menu_na;
	fld->wait_all_release = false;

	FOR (i, 4) {
		fld->sm.spr[i].init ((std::string("assets/textures/sm_") + sm_file_names[i]).c_str(), 30, 30);
		fld->sm.spr[i].itself.setPosition (MY_WIND_WIDTH / 2 - 15, 7);
	}
	fld->sm.state = SMILEY_NONE;

	field_reset (fld);

	read_string ();
	return (char *)fld;
}

kb::Key bbbb = kb::A;

O_UPDATE (field_update) {
	CNTRL ("field_update");
	O_DECL (field, fld);
	if (!(fld->win || fld->game_over)) {
        if (!fld->empty) {
		    fld->time += dt;
        }
		fld->sec.setString (std::to_string (Min (int (fld->time), 999)));
		fld->sec.setPosition(MY_WIND_WIDTH - 12 - fld->sec.getGlobalBounds ().width,9);
	}
	//fld->crt.update ();

	v2i m;
	get_mouse_pos (m);
	if (fld->menu.update (dt, v2f(m))) {
		control *mc = (control *)main_control;
		mc->todo.push_back (instruction ("delete", "L0"));
		mc->todo.push_back (instruction ("load", "MENU"));
		mc->todo.push_back (instruction ("active", "MENU"));
	}

	if (input.mbutton[MOUSE_LEFT].just_pressed) {
		if (fld->sm.spr[0].itself.getGlobalBounds ().contains (v2f(m))) {
			field_reset (fld);
		}
	}
	if ( kb::isKeyPressed (bbbb)) {
		if (bbbb == kb::A) {
			bbbb = kb::B;
		} else {
			bbbb = kb::A;
		}
		FOR_2D (v, WWW, HHH) {
			MK_C (c, v);
			if ((c->flags & CELL_FLAGS_CLOSED) && (c->flags & CELL_P_SAFE)) {
				field_open_cell (fld, v);
				return false;
			}
		}
	}
	if (input.mbutton[MOUSE_MIDDLE].just_released ||
		(input.mbutton[MOUSE_LEFT].pressed_now && input.mbutton[MOUSE_RIGHT].just_released) ||
		(input.mbutton[MOUSE_RIGHT].pressed_now && input.mbutton[MOUSE_LEFT].just_released) ||
		(input.mbutton[MOUSE_RIGHT].just_released && input.mbutton[MOUSE_LEFT].just_released)) {
			if (!fld->game_over && sf::Rect <int> (6, 43, 32*fld->gp.width, 32*fld->gp.height).contains (m) ) {
				v2i choose ((m.x - 6) / 32, (m.y - 43) / 32);
				field_double_mouse (fld, choose);
			}
			fld->wait_all_release = true;
	} else if (! (fld->win || fld->game_over)) {
		if (input.mbutton[MOUSE_LEFT].just_released && !fld->wait_all_release) {
			if (!fld->game_over && sf::Rect <int> (6, 43, 32*fld->gp.width, 32*fld->gp.height).contains (m) ) {
				v2i choose ((m.x - 6) / 32, (m.y - 43) / 32);
				if (fld->empty) {
					fld->empty = false;
					std::list <v2i> fc;
					int more;
					if (fld->gp.mines < fld->gp.height * fld->gp.width - 9) {
						more = 1;
					} else {
						more = 0;
					}
					FOR (j, fld->gp.height) {
						FOR (i, fld->gp.width) {
							if ((abs (i - choose.x) > more) || (abs (j - choose.y) > more)) {
								fc.push_back (v2i (i, j));
							}
						}
					}
					field_make (fld, &fc, fld->gp.mines);
				}
				field_open_cell (fld, choose);
			}
		}
		if (input.mbutton[MOUSE_RIGHT].just_released  && !fld->wait_all_release) {
			if (!fld->game_over && sf::Rect <int> (6, 43, 32*fld->gp.width, 32*fld->gp.height).contains (m) ) {
				v2i choose ((m.x - 6) / 32, (m.y - 43) / 32);
				MK_C (c, choose);
				if (c->flags & CELL_FLAGS_CLOSED) {
					if (c->flags & CELL_FLAGS_MARK) {
						c->flags &= ~CELL_FLAGS_MARK;
					} else {
						c->flags |= CELL_FLAGS_MARK;
					}
				}
			}
		}
	}
	if (!(input.mbutton[MOUSE_LEFT].pressed_now || input.mbutton[MOUSE_RIGHT].pressed_now)) {
		fld->wait_all_release = false;
	}

	field_check_win (fld);
	field_count_mines_left (fld);

	field_cells_upd ((char *)fld, dt,
		(input.mbutton[MOUSE_LEFT].pressed_now && input.mbutton[MOUSE_RIGHT].pressed_now) || input.mbutton[MOUSE_MIDDLE].pressed_now ? v2i((m.x - 6) / 32, (m.y - 43) / 32) : v2i (-10, -10));

	int &ip = fld->sm.state;
	ip = SMILEY_NONE;
	get_mouse_pos (m);
	if (input.mbutton[MOUSE_LEFT].pressed_now && m.y > 37) {
		ip = SMILEY_DANGER;
	}
	if (fld->game_over) {
		ip = SMILEY_GAME_OVER;
	}
	if (fld->win) {
		ip = SMILEY_WIN;
	}
	
	return false;
}

O_RENDER (field_render) {
	O_DECL (field, fld);

	window.draw (fld->bg);
	window.draw (fld->cells);
	window.draw (fld->digits, &fld->digits_text);
	window.draw (fld->forced_cells);
	window.draw (fld->mines, &fld->mine_text);
	window.draw (fld->flags, &fld->flag_text);
	window.draw (fld->crs, &fld->crs_text);
	fld->menu.draw (&window);
	fld->sm.spr[fld->sm.state].draw (&window);
	window.draw (fld->sec);
	window.draw (fld->mines_left);
	//fld->crt.render ();
}

O_DEL (field_del) {
	O_DECL (field, fld);
	delete fld;
}

std_obj_functions field_f (field_load, field_update, field_render, field_del);