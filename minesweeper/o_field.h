#pragma once
#include <vector>
#include "object.h"
#include "sfml.h"
#include <list>
#include "some_structures.h"
#include "save.h"
#include "cell.h"

class field {
public:
	cell_ar a;
	game_parameters gp;
    sf::RectangleShape bg;
	std::list <force_cell> fc_l;
	sq_button menu;
	sprite menu_na;
	sprite menu_a;
	sf::Texture mine_text;
	sf::Texture digits_text;
	sf::VertexArray cells;
	sf::VertexArray mines;
	sf::VertexArray digits;
	bool empty;
	explicit field (int h, int w) : a (h,w) { }
};

extern std_obj_functions field_f;