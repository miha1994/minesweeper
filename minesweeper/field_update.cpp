#include "o_field.h"
#include "o_control.h"
#include "space.h"
#include "save.h"
#include "quad.h"
#include "field_utility.h"
#include "input.h"
#include "animation.h"

#undef fld

extern std::string sm_file_names [4];
kb::Key bbbb = kb::A;

O_UPDATE (field_update) {
	CNTRL ("field_update");
	O_DECL (field, fld);
	if (fld->game_over || fld->win ()) {
		window.setFramerateLimit (100);
	} else {
		window.setFramerateLimit (60);
		records.close ();
	}
	if (fld->win ()) {
		fld->show_results.is_dead = false;
	} else {
		fld->show_results.is_dead = true;
		fld->show_results.is_available_to_be_pressed = true;
	}
	records.update (dt);
    fld->reallocation_time += dt;
    if (false) {//input.kb_abc['Q'-'A'].just_pressed) {
        fld->commit->checkout ();
    }
    if (false) {//input.kb_abc['W'-'A'].just_pressed) {
        fld->checkout ();
    }
	if (input.kb_abc['S'-'A'].just_pressed && fld->history_is_writing) {
		fld->hist.store ();
	}
	if (input.kb_abc['O'-'A'].pressed_now && input.kb_abc['I'-'A'].pressed_now && input.kb_abc['U'-'A'].pressed_now) {
		fld->history_is_writing = false;
		field_reset (fld);
		fld->hist.load ();
	}
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
    v2f m;
	get_mouse_pos (m);
	if (fld->show_results.update (dt, m) && fld->mm.m_time > 0.1) {
		SV_info s;
		get_save_status (s);
		table_stat ts = s.statistic[fld->gp];
		records.show (ts, fld->time, fld->reallocation_count.get_inf_value ());
		fld->show_results.is_available_to_be_pressed = false;
		fld->show_results.is_dead = true;
	}
    if (fld->menu.update (dt, v2f(m))) {
		control *mc = (control *)main_control;
		mc->todo.push_back (instruction ("delete", "L0"));
		mc->todo.push_back (instruction ("load", "MENU"));
		mc->todo.push_back (instruction ("active", "MENU"));
	}

    if (!fld->b_switch_safe_opening_ability || fld->win () || fld->game_over || fld->state == FIELD_STATE_INTRO) {
        fld->hint.is_available_to_be_pressed = false;
    } else {
        fld->hint.is_available_to_be_pressed = true;
    }
    if (fld->hint.update (dt, v2f (m))) {
        fld->use_hint ();
    }
    fld->tooltip_for_mines.setActive (fld->mines_left.getGlobalBounds ().contains (v2f(sf::Mouse::getPosition (window))));
    fld->tooltip_for_mines.update (dt);

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
			fld->time = 0;
		}
		break;
	case FIELD_STATE_IN_GAME:
		/*
		FOR_2D (v, WWW, HHH) {
			if (fld->a[v].val != fld->a[v.y][v.x].val) {
				exit (0);    // NEVER WILL EXECUTE THIS !!!!!!!!!!!!
			}
		}
		*/
		if (!(fld->win () || fld->game_over)) {
			if (!fld->empty) {
				fld->time += dt;
				fld->swag_time += dt;
			}
			fld->sec.setCharacterSize(20 * PIX);
			fld->sec.setString (Tstr (int (fld->time)));
			//fld->sec.setString (std::to_string (Min (int (fld->time), 999)));
			fld->sec.setPosition(int((MY_WIND_WIDTH - 12)*PIX - fld->sec.getGlobalBounds ().width) + fld->shift,int(6.4*PIX));
		}
		//fld->crt.update ();

        if (input.mbutton[MOUSE_LEFT].just_pressed) {
		    if (fld->sm.spr[0][PX_1].itself.getGlobalBounds ().contains (v2f(m))) {
			    field_reset (fld);
		    }
	    }
		fld->tooltip_for_sm.setActive (fld->sm.spr[0][PX_1].itself.getGlobalBounds ().contains (v2f(m)));
		fld->tooltip_for_sm.update (dt);

		m.x -= fld->shift;
		m.y -= fld->vshift;
		m /= PIX;
		
		if (false) {// kb::isKeyPressed (bbbb) || kb::isKeyPressed (kb::Z)) {
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
            /*
			if (!fld->empty) {
				FOR_2D (v, WWW, HHH) {
					MK_C (c, v);
					if ((c->flags & CELL_FLAGS_CLOSED) && !(c->flags & CELL_FLAGS_MINE)) {
						field_open_cell (fld, v);
						return false;
					}
				}
			}
			*/
		}

		if (input.mbutton[MOUSE_MIDDLE].just_released ||
			(input.mbutton[MOUSE_LEFT].pressed_now && input.mbutton[MOUSE_RIGHT].just_released) ||
			(input.mbutton[MOUSE_RIGHT].pressed_now && input.mbutton[MOUSE_LEFT].just_released) ||
			(input.mbutton[MOUSE_RIGHT].just_released && input.mbutton[MOUSE_LEFT].just_released)
			|| !fld->history_is_writing && input.kb_abc['N' - 'A'].just_pressed && !fld->hist.empty () && fld->hist.is_next_click_double ()) {
				if (!fld->game_over && sf::Rect <int> (6, 43, 32*fld->gp.width, 32*fld->gp.height).contains (v2i(m)) ) {
					v2i choice ((m.x - 6) / 32, (m.y - 43) / 32);
					if (fld->history_is_writing == false  && !fld->hist.empty ()) {
						choice = fld->hist.get_next_point ();
					} else {
						fld->hist.push_double_click (choice);
					}
					field_double_mouse (fld, choice);
				}
				fld->wait_all_release = true;
		} else if (! (fld->win () || fld->game_over)) {
			if (input.mbutton[MOUSE_LEFT].just_released && !fld->wait_all_release
				|| !fld->history_is_writing && (input.kb_abc['N' - 'A'].just_pressed || input.kb_abc['M' - 'A'].pressed_now) && !fld->hist.empty () && fld->hist.is_next_click_left ()) {
				if (!fld->game_over && sf::Rect <int> (6, 43, 32*fld->gp.width, 32*fld->gp.height).contains (v2i(m)) ) {
					v2i choice ((m.x - 6) / 32, (m.y - 43) / 32);
					if (fld->history_is_writing == false && !fld->hist.empty ()) {
						choice = fld->hist.get_next_point ();
					}

					if (fld->empty) {
						if (fld->history_is_writing) {
							fld->hist.push_left_click (choice);
						}
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
								if ((abs (i - choice.x) > more) || (abs (j - choice.y) > more)) {
									fc.push_back (v2i (i, j));
								}
							}
						}
						field_make (fld, &fc, fld->gp.mines);
					}
					
					field_open_cell (fld, choice);
				}
			}
			if (input.mbutton[MOUSE_RIGHT].just_pressed || (input.mbutton[MOUSE_LEFT].just_pressed && input.mbutton[MOUSE_RIGHT].pressed_now
				|| !fld->history_is_writing && input.kb_abc['N' - 'A'].just_pressed && !fld->hist.empty () && fld->hist.is_next_click_right ())) {
				if (!fld->game_over && sf::Rect <int> (6, 43, 32*fld->gp.width, 32*fld->gp.height).contains (v2i(m)) ) {
					v2i choice ((m.x - 6) / 32, (m.y - 43) / 32);
					if (fld->history_is_writing == false && !fld->hist.empty ()) {
						choice = fld->hist.get_next_point ();
					}
					MK_C (c, choice);
					
					if (c->flags & CELL_FLAGS_CLOSED) {
						if (fld->history_is_writing) {
							fld->hist.push_right_click (choice);
						}
						if (c->flags & CELL_FLAGS_MARK) {
							c->flags &= ~CELL_FLAGS_MARK;
							if (fld->q_is_enabled) {
								c->flags |= CELL_FLAGS_Q;
							}
						} else if (c->flags & CELL_FLAGS_Q) {
							TRN_OFF (c->flags, CELL_FLAGS_Q);
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
		if (!fld->win ()) {
			field_check_win (fld);
			if (fld->win ()) {
				fld->mm.init ();
			}
		}
		field_count_mines_left (fld);

		field_cells_upd ((char *)fld, dt,
			(input.mbutton[MOUSE_LEFT].pressed_now && input.mbutton[MOUSE_RIGHT].pressed_now)
			|| input.mbutton[MOUSE_MIDDLE].pressed_now ? v2i((m.x - 6) / 32, (m.y - 43) / 32) : v2i (-10, -10),
			v2f((m.x - 6) / 32.0, (m.y - 43) / 32.0) );

		int &ip = fld->sm.state;
		ip = SMILEY_NONE;
		get_mouse_pos (m);
		if (input.mbutton[MOUSE_LEFT].pressed_now && m.y > 37 * PIX) {
			ip = SMILEY_DANGER;
		}
		if (fld->game_over) {
			ip = SMILEY_GAME_OVER;
            fld->win_time += dt;
		}
		if (fld->win ()) {
			ip = SMILEY_WIN;
			fld->win_time += dt;
			float x;

			switch (fld->wm.state) {
			case 0:
				x = pow(3 - fld->win_time, 2) * (10);
				fld->wm.spr[PX_1].itself.setPosition (v2f (fld->shift, fld->vshift) + v2f( v2f(6, 43) + v2f(x, HHH - 4)*32.0f ) * PIX);
				if (fld->win_time >= 3) {
					fld->win_time = 0;
					fld->wm.state = 1;
					fld->wm.spr[PX_1].itself.setPosition (v2f (fld->shift, fld->vshift) + v2f( v2f(6, 43) + v2f(0, HHH - 4)*32.0f ) * PIX);
				}
				break;
			case 1:
				fld->wm.spr[PX_1].itself.setPosition (v2f (fld->shift, fld->vshift) + v2f( v2f(6, 43) + v2f(0, HHH - 4)*32.0f ) * PIX);
				break;
			case 2:
				x = pow(fld->win_time, 2) * (-10);
				fld->wm.spr[PX_1].itself.setPosition (v2f (fld->shift, fld->vshift) + v2f( v2f(6, 43) + v2f(x, HHH - 4)*32.0f ) * PIX);
				if (fld->win_time >= 2) {
					fld->win_time = 0;
					fld->wm.state = 3;
					fld->wm.spr[PX_1].itself.setPosition (v2f (fld->shift, fld->vshift) + v2f( v2f(6, 43) + v2f(-1000, HHH - 4)*32.0f ) * PIX);
				}
				break;
			case 3:
				fld->wm.spr[PX_1].itself.setPosition (v2f (fld->shift, fld->vshift) + v2f( v2f(6, 43) + v2f(-1000, HHH - 4)*32.0f ) * PIX);
			}
			if (fld->mm.update (dt*1.5)) {
				fld->wm.state = 2;
				fld->win_time = 0;
			}
		}
		break;
	}
	
	return false;
}

#define SZ1				sizeof (int), 1, f
#define write(f, what)	fwrite (&(what), sizeof(what), 1, f)
#define read(f, what)	fread (&(what), sizeof(what), 1, f)

#define fld this

void field::save (int const_) {
	std::string file_name = "assets/inf/" + Tstr(gp.height) + "_" + Tstr(gp.width) + "_" + Tstr(gp.mines) + ".stt";
	if (fld->state == FIELD_STATE_IN_GAME && fld->win () == false && fld->game_over == false) {
		FILE *f = fopen (file_name.c_str (), "wb");
		write (f, time);
		int rc = reallocation_count.get_inf_value ();
		write (f, rc);
		FOR_2D (v, WWW, HHH) {
			MK_C (c, v);
			write (f, c->flags);
			write (f, c->val);
		}
		fclose (f);
	} else {
		unlink (file_name.c_str ());
	}
}

bool field::load () {
	std::string file_name = "assets/inf/" + Tstr(gp.height) + "_" + Tstr(gp.width) + "_" + Tstr(gp.mines) + ".stt";
	FILE *f = fopen (file_name.c_str (), "rb");
	if (!f) {
		return false;
	}
	read (f, time);
	int rc;
	read (f, rc);
	reallocation_count.set_inf_value (rc);
	FOR_2D (v, WWW, HHH) {
		MK_C (c, v);
		read (f, c->flags);
		read (f, c->val);
	}
	fclose (f);
	unlink (file_name.c_str ());
	fld->no_moves = true;
	FOR_2D (v, WWW, HHH) {
		MK_C (c, v);
		if ((c->flags & CELL_FLAGS_CLOSED) && (c->flags & CELL_P_SAFE)) {
			fld->no_moves = false;
		}
	}
	return true;
}

field::~field () {
	save ();
}