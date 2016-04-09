#pragma once
#include <vector>
#include "object.h"
#include "sfml.h"
#include <list>
#include "some_structures.h"
#include "save.h"
#include "cell.h"
#include "tooltip.h"
#include "table_stat.h"
#include "history_of_attempt.h"

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

struct explosions {
	void *field_;
	struct elem {
		v2i pos;
		int state;
		float time;
		float time_when_state_will_be_changed;
		elem () {}
	};
	int N;
	int rnd;
	std::list <elem> list_of_explosions;
	sf::VertexArray circles;
	void init (void *fld_á, v2i center);
	void update (float dt);
	void draw (sf::RenderWindow *wind);
};

class field {
	bool z_win_;
public:
	int state;
	int mkr;
	smiley sm;
	win_s wm;
	history hist;
	mines_moving mm;
	explosions expl;
	cell_ar<cell> a;
	game_parameters gp;
    sf::RectangleShape bg;
	std::list <force_cell> fc_l;
	std::set <v2i> new_digits;
	sq_button menu;
    sprite menu_na[2];
	sprite menu_a[2];
    sq_button hint;
    sprite hint_na[2];
    sprite hint_a[2];
	sq_button show_results;
	sprite spr_show_results[2];
    tooltip tooltip_for_mines;
	tooltip tooltip_for_sm;
	sf::Texture mine_text[2];
	sf::Texture digits_text[2];
	sf::Texture flag_text[2];
	sf::Texture crs_text[2];
	sf::Texture cell_text[2];
	sf::Texture free_text[2];
	sf::Texture q_text[2];
	sf::VertexArray cells;
	sf::VertexArray bg_cells;
	sf::VertexArray forced_cells;
	sf::VertexArray flags;
	sf::VertexArray mines;
	sf::VertexArray digits;
	sf::VertexArray crs;
	sf::VertexArray free;
	sf::VertexArray q_array;
	sf::Font font;
	sf::Text mines_left;
	sf::Text sec;
    text_one_type <int> reallocation_count;
	v2i fail_choice;
	float time;
	float pix;
	float shift;
	float vshift;
	float win_time;
    float reallocation_time;
	int r_wind_w;
	int r_wind_h;
	int intro_num;
	int inside_w;
	bool empty;
	bool game_over;
	bool wait_all_release;
	bool no_moves;
    bool b_switch_mine_moving_ability;
    bool b_switch_safe_opening_ability;
	bool q_is_enabled;
	bool history_is_writing;
	change_real_time crt;
    field *commit;
	explicit field (int h, int w) : a (h,w) { }
    field (field &fld);
    void checkout ();
    void use_hint ();
    void fail (v2i bad_choice);
	bool win () {return z_win_;}
	void set_win (bool val) {z_win_ = val;}
	void save ();
	bool load ();
	~field ();
};

extern std_obj_functions field_f;