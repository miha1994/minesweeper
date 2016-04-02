#include "o_control.h"
#include "space.h"
#include "save.h"
#include "quad.h"
#include "field_utility.h"
#include "input.h"
#include "animation.h"

extern std::string sm_file_names [4];

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
    fld->tooltip_for_mines.setString ("Mines left");
	fld->tooltip_for_sm.setString ("Reset");
    fld->reallocation_time = 10;
	fld->mm.fld_ = (char *) fld;

	fld->font.loadFromFile ("assets/fonts/cg.ttf");

	fld->sec.setFont (fld->font);
	fld->sec.setColor (my_clr_s[color_theme].font);
	fld->sec.setString ("999");
	fld->sec.setCharacterSize (20);

	fld->mines_left.setFont (fld->font);
	fld->mines_left.setColor (my_clr_s[color_theme].font);
	fld->mines_left.setString ("999");
	fld->mines_left.setCharacterSize (20);

    fld->reallocation_count->setFont (fld->font);
	fld->reallocation_count->setColor (my_clr_s[color_theme].font);
	fld->reallocation_count->setString ("999");
	fld->reallocation_count->setCharacterSize (20);
    fld->reallocation_count.origin_type.set__xy_is_top_right_point ();

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
	fld->free.setPrimitiveType (sf::Quads);
	
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
		fld->free_text[i].loadFromFile ("assets/textures/free" + (!i ? std::string("_48") : std::string("")) + ".png");
		fld->free_text[i].setSmooth (true);
	}

	fld->menu_na[0].init ("assets/textures/menu_na_48.png", 78, 48, true);
	fld->menu_a[0].init ("assets/textures/menu_a_48.png", 78, 48, true);
	fld->menu_na[1].init ("assets/textures/menu_na.png", 156, 96, true);
	fld->menu_a[1].init ("assets/textures/menu_a.png", 156, 96, true);

	fld->menu.alpha = 0;
    fld->menu.tip.setString ("Back to menu");
    fld->menu.tip_using = true;

    fld->hint_na[0].init ("assets/textures/hint_na_48.png", 78, 48, true);
	fld->hint_a[0].init ("assets/textures/hint_a_48.png", 78, 48, true);
	fld->hint_na[1].init ("assets/textures/hint_na.png", 156, 96, true);
	fld->hint_a[1].init ("assets/textures/hint_a.png", 156, 96, true);
	
    fld->hint.alpha = 0;
    SV_info sv;
    get_save_status (&sv);
    fld->b_switch_mine_moving_ability = sv.Mine_moving_ability;
    fld->b_switch_safe_opening_ability = sv.Safe_opening_ability;
    if (fld->b_switch_safe_opening_ability) {
        fld->hint.tip.setString ("Open mine-free cell");
    } else {
        fld->hint.tip.setString ("Number of mines reallocations");
    }
    fld->hint.tip_using = true;

    fld->hint.is_dead = !(fld->b_switch_safe_opening_ability || fld->b_switch_mine_moving_ability);
	fld->wait_all_release = false;

	FOR (i, 4) {
		FOR (z, 2) {
			fld->sm.spr[i][z].init ((std::string("assets/textures/sm_") + sm_file_names[i] + (z ? "" : "_48") + ".png").c_str(), z ? 96 : 48, z ? 96 : 48, true);
		}
	}
	fld->sm.state = SMILEY_NONE;

	FOR (z, 2) {
		fld->wm.spr[z].init ((std::string("assets/textures/wtruck") + (z ? "" : "_48") + ".png").c_str(), 4*(z ? 96 : 48), 4*(z ? 96 : 48), true);
		fld->wm.spr[z].itself.setColor (my_clr_s[color_theme].win_man);
	}

	field_reset (fld);
	field_ch_pix (fld);

	read_string ();
    fld->commit = new field (*fld);
	return (char *)fld;
}