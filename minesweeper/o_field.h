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
	sprite spr[4];
	int state;
};

class field {
public:
	smiley sm;
	cell_ar<cell> a;
	game_parameters gp;
    sf::RectangleShape bg;
	std::list <force_cell> fc_l;
	sq_button menu;
	sprite menu_na;
	sprite menu_a;
	sf::Texture mine_text;
	sf::Texture digits_text;
	sf::Texture flag_text;
	sf::Texture crs_text;
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
	bool empty;
	bool game_over;
	bool win;
	bool wait_all_release;
	bool no_moves;
	change_real_time crt;
	explicit field (int h, int w) : a (h,w) { }
};

extern std_obj_functions field_f;