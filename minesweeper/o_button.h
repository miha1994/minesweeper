#pragma once
#include <vector>
#include "object.h"
#include "sfml.h"
#include <list>
#include "some_structures.h"
#include "save.h"
#include "tooltip.h"

struct cell_s {
	int value;
	bool active;
	sf::Text txt;
	float time;
	v2i pos;
	int max_val;
    int min_val;
	void upd (int new_val) {
		value = new_val > max_val ? max_val : new_val;
		txt.setString(value > 0 ? Tstr (value) : "");
		txt.setPosition (pos.x - txt.getGlobalBounds ().width, pos.y);
	}
    void upd_min () {
        if (value < min_val) {
            value = min_val;
            txt.setString(value > 0 ? Tstr (value) : "");
		    txt.setPosition (pos.x - txt.getGlobalBounds ().width, pos.y);
        }
    }
};

class button {
public:
    change_real_time crt;
	SV_info save;
	vec <sf::Text> mode_table;
	sprite play_active;
	sprite play_not_active;
	sprite add_na;
	sprite add_a;
    sprite bool_a;
    sprite bool_na;
    sprite del_active;
    sprite del_not_active;

	vec <sq_button> play_buttons;
    vec <sq_button> del_buttons;
	sq_button add;
    option_switcher sw_map_realloc;
    option_switcher sw_open_mine_free_cell;
    sf::Font font;
	sf::Text height;
    sf::Text width;
	sf::Text Custom_mode;
    sf::RectangleShape bg;
	sf::RectangleShape cell;
	sf::Vertex line[2];
	sf::VertexArray cells;
	sprite mine;
	int x[4];
	int y[3];
	cell_s hwm[3];
}; 

void make_table (button *btn);

extern std_obj_functions button_f;