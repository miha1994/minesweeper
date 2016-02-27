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
	fld->mcr = 0;
	fld->no_moves = false;
	fld->state = FIELD_STATE_INTRO;
}

void field_ch_pix (field *fld) {
	fld->menu.pos = v2f (6, 3.5) * PIX + v2f (fld->shift, 0);
	fld->menu.act = &fld->menu_a[PX_1];
	fld->menu.spr = &fld->menu_na[PX_1];
	float scale = (fld->pix / PX_96) * (28.0 / 30);
	fld->menu.act->itself.setScale (scale, scale);
	fld->menu.spr->itself.setScale (scale, scale);

	FOR (i, 4) {
		fld->sm.spr[i][PX_1].itself.setPosition ((MY_WIND_WIDTH / 2 - 16) * PIX + fld->shift, 3.5 * PIX);
		fld->sm.spr[i][PX_1].itself.setScale ((fld->pix / PX_96) * (28.0 / 30), (fld->pix / PX_96) * (28.0 / 30));
	}
	fld->mines_left.setCharacterSize(20 * PIX);
	fld->mines_left.setPosition (int(90*PIX - fld->mines_left.getGlobalBounds ().width) + fld->shift , int(7*PIX));
	fld->sec.setCharacterSize(20 * PIX);
	fld->sec.setPosition(int((MY_WIND_WIDTH - 12)*PIX - fld->sec.getGlobalBounds ().width) + fld->shift,int(6.4*PIX));	
	fld->bg.setSize (v2f(fld->inside_w, R_WIND_HEIGHT - (fld->vshift > 1 ? 37.0 * PIX : 0)));
	fld->bg.setPosition (fld->shift,fld->vshift + (fld->vshift > 1 ? 37.0 * PIX : 0));
}

O_LOAD (field_load) {
	field *fld = new field (global_game_parameters.height, global_game_parameters.width);
	fld->gp = global_game_parameters;
	fld->pix = 32;
	fld->shift = 0;
	fld->vshift = 0;
	fld->intro_num = -1;
	fld->r_wind_h = R_WIND_HEIGHT;
	fld->inside_w =
	fld->r_wind_w = R_WIND_WIDTH;

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

	fld->bg.setPosition (fld->shift,fld->vshift);
	fld->bg.setSize (v2f(MY_WIND_WIDTH, MY_WIND_HEIGHT));
	fld->bg.setFillColor (my_clr_s[color_theme].bg);

	fld->cells.setPrimitiveType (sf::Quads);
	fld->forced_cells.setPrimitiveType (sf::Quads);
	fld->mines.setPrimitiveType (sf::Quads);
	fld->digits.setPrimitiveType (sf::Quads);
	fld->flags.setPrimitiveType (sf::Quads);
	fld->crs.setPrimitiveType (sf::Quads);
	
	FOR (i, 2) {
		fld->digits_text[i].loadFromFile ("assets/textures/digits" + (!i ? std::string("_48") : std::string("")) + ".png");
		fld->digits_text[i].setSmooth (true);
		fld->mine_text[i].loadFromFile ("assets/textures/mine1" + (!i ? std::string("_48") : std::string("")) + ".png");
		fld->mine_text[i].setSmooth (true);
		fld->flag_text[i].loadFromFile ("assets/textures/flag" + (!i ? std::string("_48") : std::string("")) + ".png");
		fld->flag_text[i].setSmooth (true);
		fld->crs_text[i].loadFromFile ("assets/textures/cross" + (!i ? std::string("_48") : std::string("")) + ".png");
		fld->crs_text[i].setSmooth (true);
		fld->cell_text[i].loadFromFile ("assets/textures/cell" + (!i ? std::string("_48") : std::string("")) + ".png");
		fld->cell_text[i].setSmooth (true);
	}

	fld->menu_na[0].init ("assets/textures/menu_na_48.png", 78, 48, true);
	fld->menu_a[0].init ("assets/textures/menu_a_48.png", 78, 48, true);
	fld->menu_na[1].init ("assets/textures/menu_na.png", 156, 96, true);
	fld->menu_a[1].init ("assets/textures/menu_a.png", 156, 96, true);

	fld->menu.alpha = 0;
	
	fld->wait_all_release = false;

	FOR (i, 4) {
		FOR (z, 2) {
			fld->sm.spr[i][z].init ((std::string("assets/textures/sm_") + sm_file_names[i] + (z ? "" : "_48") + ".png").c_str(), z ? 96 : 48, z ? 96 : 48, true);
		}
	}
	fld->sm.state = SMILEY_NONE;

	field_reset (fld);
	field_ch_pix (fld);

	read_string ();
	return (char *)fld;
}

kb::Key bbbb = kb::A;

O_UPDATE (field_update) {
	CNTRL ("field_update");
	O_DECL (field, fld);
	int inside_w;
	if (fld->r_wind_h != R_WIND_HEIGHT || fld->r_wind_w != R_WIND_WIDTH) {
		v2i wpos = window.getPosition ();
		int pos_changed = false;
		int old_w = R_WIND_WIDTH;
		int old_h = R_WIND_HEIGHT;
		if (R_WIND_WIDTH > SC_WIDTH - 20 && R_WIND_HEIGHT > SC_HEIGHT - 120) {
			if (float (R_WIND_WIDTH) / R_WIND_HEIGHT > float (MY_WIND_WIDTH) / MY_WIND_HEIGHT) {
				int old = R_WIND_WIDTH; 
				inside_w = R_WIND_WIDTH = (float (MY_WIND_WIDTH) / MY_WIND_HEIGHT) * R_WIND_HEIGHT;
				fld->shift = (old - R_WIND_WIDTH) / 2;
				fld->vshift = 0;
			} else {
				inside_w = R_WIND_WIDTH;
				R_WIND_HEIGHT = (float (MY_WIND_HEIGHT) / MY_WIND_WIDTH) * R_WIND_WIDTH;
				fld->shift = 0;
				fld->vshift = (old_h - R_WIND_HEIGHT) / 2;
			}
			R_WIND_HEIGHT = old_h;
			R_WIND_WIDTH = old_w;
		} else {
			R_WIND_WIDTH = Max (R_WIND_WIDTH, 8 + WWW * 24);
			R_WIND_HEIGHT = Max (R_WIND_HEIGHT, 34 + HHH * 24);
			if (R_WIND_HEIGHT == fld->r_wind_h && R_WIND_WIDTH != fld->r_wind_w) {
				wpos.y -= ((float (MY_WIND_HEIGHT) / MY_WIND_WIDTH) * R_WIND_WIDTH - R_WIND_HEIGHT) / 2;
				pos_changed = 1;
				R_WIND_HEIGHT = (float (MY_WIND_HEIGHT) / MY_WIND_WIDTH) * R_WIND_WIDTH;
				fld->shift = 0;
				fld->vshift = 0;
			} else {
				wpos.x -= ((float (MY_WIND_WIDTH) / MY_WIND_HEIGHT) * R_WIND_HEIGHT - R_WIND_WIDTH) / 2;
				R_WIND_WIDTH = (float (MY_WIND_WIDTH) / MY_WIND_HEIGHT) * R_WIND_HEIGHT;
				pos_changed = 1;
				fld->shift = 0;
				fld->vshift = 0;
			}
			inside_w = R_WIND_WIDTH;
		}
		fld->inside_w = inside_w;
		window.setSize (v2u(R_WIND_WIDTH, R_WIND_HEIGHT));
		window.setView (sf::View (sf::FloatRect(0,0,R_WIND_WIDTH, R_WIND_HEIGHT)));
		if (pos_changed) {
			window.setPosition (wpos);
		}
        //window.create (sf::VideoMode (old_w, old_h), "Minesweeper", sf::Style::Default);
		//window.setPosition (wpos);
	    window.setVerticalSyncEnabled (true);
	    window.setFramerateLimit (60);

		fld->pix = float(inside_w*(fld->gp.width * 32) / (float (fld->gp.width * 32 + 12))) / fld->gp.width;
		field_ch_pix (fld);
		fld->r_wind_h = R_WIND_HEIGHT;
		fld->r_wind_w = R_WIND_WIDTH;
	}
	switch (fld->state) {
	case FIELD_STATE_INTRO:
		fld->mines_left.setCharacterSize(20 * PIX);
		fld->mines_left.setString (std::to_string (fld->gp.mines));
		fld->mines_left.setPosition (int(90*PIX - fld->mines_left.getGlobalBounds ().width) + fld->shift, int(7*PIX));
		fld->sec.setCharacterSize(20 * PIX);
		fld->sec.setString (std::to_string (0));
		fld->sec.setPosition(int((MY_WIND_WIDTH - 12)*PIX - fld->sec.getGlobalBounds ().width) + fld->shift,int(6.4*PIX));
		if (fld->intro_num < 0) {
			fld->intro_num = rand () % 5;
		}
		if (intro_anim[fld->intro_num] (fld, dt)) {
			fld->state = FIELD_STATE_IN_GAME;
			fld->intro_num = -1;
		}
		break;
	case FIELD_STATE_IN_GAME:
		if (!(fld->win || fld->game_over)) {
			if (!fld->empty) {
				fld->time += dt;
			}
			fld->sec.setCharacterSize(20 * PIX);
			fld->sec.setString (std::to_string (Min (int (fld->time), 999)) + "   " + Tstr (fld->mcr / 1000000.0));
			fld->sec.setPosition(int((MY_WIND_WIDTH - 12)*PIX - fld->sec.getGlobalBounds ().width) + fld->shift,int(6.4*PIX));
		}
		//fld->crt.update ();

		v2f m;
		get_mouse_pos (m);
		if (fld->menu.update (dt, v2f(m))) {
			control *mc = (control *)main_control;
			mc->todo.push_back (instruction ("delete", "L0"));
			mc->todo.push_back (instruction ("load", "MENU"));
			mc->todo.push_back (instruction ("active", "MENU"));
		}

		if (input.mbutton[MOUSE_LEFT].just_pressed) {
			if (fld->sm.spr[0][PX_1].itself.getGlobalBounds ().contains (v2f(m))) {
				field_reset (fld);
			}
		}
		m.x -= fld->shift;
		m.y -= fld->vshift;
		m /= PIX;
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
			if (!fld->empty) {
			FOR_2D (v, WWW, HHH) {
				MK_C (c, v);
				if ((c->flags & CELL_FLAGS_CLOSED) && !(c->flags & CELL_FLAGS_MINE)) {
					field_open_cell (fld, v);
					return false;
				}
			}
			}
		}
		if (input.mbutton[MOUSE_MIDDLE].just_released ||
			(input.mbutton[MOUSE_LEFT].pressed_now && input.mbutton[MOUSE_RIGHT].just_released) ||
			(input.mbutton[MOUSE_RIGHT].pressed_now && input.mbutton[MOUSE_LEFT].just_released) ||
			(input.mbutton[MOUSE_RIGHT].just_released && input.mbutton[MOUSE_LEFT].just_released)) {
				if (!fld->game_over && sf::Rect <int> (6, 43, 32*fld->gp.width, 32*fld->gp.height).contains (v2i(m)) ) {
					v2i choose ((m.x - 6) / 32, (m.y - 43) / 32);
					field_double_mouse (fld, choose);
				}
				fld->wait_all_release = true;
		} else if (! (fld->win || fld->game_over)) {
			if (input.mbutton[MOUSE_LEFT].just_released && !fld->wait_all_release) {
				if (!fld->game_over && sf::Rect <int> (6, 43, 32*fld->gp.width, 32*fld->gp.height).contains (v2i(m)) ) {
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
			if (input.mbutton[MOUSE_RIGHT].just_pressed || (input.mbutton[MOUSE_LEFT].just_pressed && input.mbutton[MOUSE_RIGHT].pressed_now)) {
				if (!fld->game_over && sf::Rect <int> (6, 43, 32*fld->gp.width, 32*fld->gp.height).contains (v2i(m)) ) {
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
		if (input.mbutton[MOUSE_LEFT].pressed_now && m.y > 37 * PIX) {
			ip = SMILEY_DANGER;
		}
		if (fld->game_over) {
			ip = SMILEY_GAME_OVER;
		}
		if (fld->win) {
			ip = SMILEY_WIN;
		}
		break;
	}
	
	return false;
}

O_RENDER (field_render) {
	O_DECL (field, fld);
	//fld->bg.setFillColor (rand () & 1 ? CLR::Black : CLR::White);
	window.draw (fld->bg);
	window.draw (fld->cells, &fld->cell_text[fld->pix > 48]);
	window.draw (fld->digits, &fld->digits_text[fld->pix > 48]);
	window.draw (fld->forced_cells, &fld->cell_text[fld->pix > 48]);
	window.draw (fld->mines, &fld->mine_text[fld->pix > 48]);
	window.draw (fld->flags, &fld->flag_text[fld->pix > 48]);
	window.draw (fld->crs, &fld->crs_text[fld->pix > 48]);
	fld->menu.draw (&window);
	fld->sm.spr[fld->sm.state][PX_1].draw (&window);
	window.draw (fld->sec);
	window.draw (fld->mines_left);
	//fld->crt.render ();
}

O_DEL (field_del) {
	O_DECL (field, fld);
	delete fld;
}

std_obj_functions field_f (field_load, field_update, field_render, field_del);