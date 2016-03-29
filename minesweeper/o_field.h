#pragma once
#include <vector>
#include "object.h"
#include "sfml.h"
#include <list>
#include "some_structures.h"
#include "save.h"
#include "cell.h"

#define HHH		fld->gp.height
#define WWW		fld->gp.width

#define SMILEY_NONE			0
#define SMILEY_DANGER		1
#define SMILEY_WIN			2
#define SMILEY_GAME_OVER	3

struct smiley {
	sprite spr[4][2];
	int state;
};

struct win_s {
	sprite spr[2];
	int state;
};

#define FIELD_STATE_INTRO	0
#define FIELD_STATE_IN_GAME 1

class field {
public:
	int state;
	int mkr;
	smiley sm;
	win_s wm;
	mines_moving mm;
	cell_ar<cell> a;
	game_parameters gp;
    sf::RectangleShape bg;
	std::list <force_cell> fc_l;
	std::set <v2i> new_digits;
	sq_button menu;
	sprite menu_na[2];
	sprite menu_a[2];
	sf::Texture mine_text[2];
	sf::Texture digits_text[2];
	sf::Texture flag_text[2];
	sf::Texture crs_text[2];
	sf::Texture cell_text[2];
	sf::VertexArray cells;
	sf::VertexArray forced_cells;
	sf::VertexArray flags;
	sf::VertexArray mines;
	sf::VertexArray digits;
	sf::VertexArray crs;
	sf::Font font;
	sf::Text mines_left;
	sf::Text sec;
	float time;
	float pix;
	float shift;
	float vshift;
	float win_time;
	int r_wind_w;
	int r_wind_h;
	int intro_num;
	int inside_w;
	bool empty;
	bool game_over;
	bool win;
	bool wait_all_release;
	bool no_moves;
	change_real_time crt;
    field *commit;
	explicit field (int h, int w) : a (h,w) { }
    field (field &fld);
    void checkout ();
};

extern std_obj_functions field_f;