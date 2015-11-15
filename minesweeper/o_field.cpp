#include "o_field.h"
#include "o_control.h"
#include "space.h"
#include "save.h"
#include "quad.h"
#include "field_utility.h"
#include "input.h"

O_LOAD (field_load) {
	field *fld = new field (global_game_parameters.height, global_game_parameters.width);
	fld->gp = global_game_parameters;

	FOR (i, fld->gp.height) {
		FOR (j, fld->gp.width) {
			fld->a[i][j].flags = CELL_FLAGS_CLOSED;
			fld->a[i][j].val = 0;
		}
	}

	fld->bg.setPosition (0,0);
	fld->bg.setSize (v2f(MY_WIND_WIDTH, MY_WIND_HEIGHT));
	fld->bg.setFillColor (my_clr_s[color_theme].bg);

	fld->cells.setPrimitiveType (sf::Quads);
	fld->mines.setPrimitiveType (sf::Quads);
	fld->digits.setPrimitiveType (sf::Quads);
	
	fld->digits_text.loadFromFile ("assets/textures/digits.png");
	fld->digits_text.setSmooth (false);
	fld->mine_text.loadFromFile ("assets/textures/mine1.png");
	fld->mine_text.setSmooth (false);

	fld->menu_na.init ("assets/textures/menu_na.png", 50, 30);
	fld->menu_a.init ("assets/textures/menu_a.png", 50, 30);
	fld->menu.alpha = 0;
	fld->menu.pos = v2i (7, 7);
	fld->menu.act = &fld->menu_a;
	fld->menu.spr = &fld->menu_na;

	fld->empty = true;

	read_string ();
	return (char *)fld;
}

O_UPDATE (field_update) {
	CNTRL ("field_update");
	O_DECL (field, fld);

	v2i m;
	get_mouse_pos (m);
	if (fld->menu.update (dt, v2f(m))) {
		control *mc = (control *)main_control;
		mc->todo.push_back (instruction ("delete", "L0"));
		mc->todo.push_back (instruction ("load", "MENU"));
		mc->todo.push_back (instruction ("active", "MENU"));
	}

	if (input.mbutton[MOUSE_LEFT].just_released) {
		if (sf::Rect <int> (6, 43, 32*fld->gp.width, 32*fld->gp.height).contains (m) ) {
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
			if (fld->a[choose].flags & CELL_FLAGS_CLOSED) {
				fld->a[choose].flags = 0;
				force_cell fc (choose, my_clr_s[color_theme].unknown, my_clr_s[color_theme].safe, 0.1);
				fld->fc_l.push_back (fc);
			}
		}
	}

	field_cells_upd ((char *)fld, dt);
	return false;
}

O_RENDER (field_render) {
	O_DECL (field, fld);

	window.draw (fld->bg);
	window.draw (fld->cells);
	window.draw (fld->mines, &fld->mine_text);
	window.draw (fld->digits, &fld->digits_text);
	fld->menu.draw (&window);
}

O_DEL (field_del) {
	O_DECL (field, fld);
	delete fld;
}

std_obj_functions field_f (field_load, field_update, field_render, field_del);