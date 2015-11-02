#pragma once
#include <vector>
#include "object.h"
#include "sfml.h"
#include <list>
#include "some_structures.h"
#include "save.h"

struct cell_s {
	int value;
	bool active;
	sf::Text txt;
	float time;
	v2i pos;
	int max_val;
	void upd (int new_val) {
		value = new_val > max_val ? max_val : new_val;
		txt.setString(value > 0 ? Tstr (value) : "");
		txt.setPosition (pos.x - txt.getGlobalBounds ().width, pos.y);
	}
};

struct sq_button {
	v2i pos;
	bool pressed_lt;
	sprite *spr;
	sprite *act;
	float alpha;
	void draw (sf::RenderWindow *wind);
	bool update (float dt, v2f mouse_pos);
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
	vec <sq_button> play_buttons;
	sq_button add;
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