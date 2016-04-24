#include "o_button.h"
#include "o_control.h"
#include "space.h"
#include "save.h"
#include "quad.h"

char *button_load (char *parent_space) {
	button *btn = new button;
    
    MY_WIND_WIDTH = 440;//f_modes[0].width;
	MY_WIND_HEIGHT = 400;//f_modes[0].height;

    window.close ();
	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;
	window.create (sf::VideoMode (MY_WIND_WIDTH, MY_WIND_HEIGHT), "Minesweeper", sf::Style::Titlebar | sf::Style::Close, settings);
	sf::Image image;
	image.loadFromFile( "assets/textures/icon.png" );
	window.setIcon( image.getSize().x, image.getSize().y, image.getPixelsPtr() );
	window.setVerticalSyncEnabled (true);
	window.setFramerateLimit (60);

    btn->font.loadFromFile ("assets/fonts/cg.ttf");
	btn->mine.init ("assets/textures/mine2.png", 25, 25);

    btn->height.setFont (btn->font);
    btn->height.setColor (my_clr_s[color_theme].font);
    btn->height.setString ("Height");
    
    btn->width.setFont (btn->font);
    btn->width.setColor (my_clr_s[color_theme].font);
    btn->width.setString ("Width");

    btn->bg.setPosition (0,0);
    btn->bg.setSize (v2f(440,400));
	btn->bg.setFillColor (CLR (253, 253, 245, 255));

	btn->cell.setFillColor (my_clr_s[color_theme].safe);
	
	btn->height.setCharacterSize (17);
	btn->width.setCharacterSize (17);

	btn->mine.itself.setColor (my_clr_s[color_theme].mine);

	btn->Custom_mode.setFont (btn->font);
	btn->Custom_mode.setColor (my_clr_s[color_theme].font);
	btn->Custom_mode.setString ("Custom mode");
	btn->Custom_mode.setCharacterSize (25);
	btn->Custom_mode.setPosition (7,0);

	btn->play_active.init ("assets/textures/play_active.png", 30, 30);
	btn->play_not_active.init ("assets/textures/play_not_active.png", 30, 30);
    btn->del_active.init ("assets/textures/del_active.png", 30, 30);
    btn->del_not_active.init ("assets/textures/del_not_active.png", 30, 30);
    btn->bool_a.init ("assets/textures/bool_a.png", 30, 30);
	btn->bool_na.init ("assets/textures/bool_na.png", 30, 30);
	btn->res_a.init ("assets/textures/res_a.png", 18, 30);
	btn->res_na.init ("assets/textures/res_na.png", 18, 30);

	btn->shadow.init ("assets/textures/shadow.png", 200, 200);

	btn->shadow.itself.setPosition (0, MY_WIND_HEIGHT - 200);

    get_save_status (btn->save);

#define DF1 btn->sw_map_realloc.get_text ()
#define DF2 btn->sw_open_mine_free_cell.get_text ()
#define DF3 btn->sw_q.get_text ()
    btn->sw_map_realloc.init ("Mine moving", &btn->font, v2f (10, 150 + 10), v2f (140, 20), &btn->bool_na, &btn->bool_a);
    btn->sw_open_mine_free_cell.init ("Safe opening", &btn->font, v2f (10, 150 + 37), v2f (140, 20), &btn->bool_na, &btn->bool_a);
	btn->sw_q.init ("Question marks", &btn->font, v2f (10, 150 + 100), v2f (150, 20), &btn->bool_na, &btn->bool_a);
    DF1.setCharacterSize (16);
    DF2.setCharacterSize (16);
	DF3.setCharacterSize (16);
    btn->sw_map_realloc.turned_on = btn->save.Mine_moving_ability;
    btn->sw_open_mine_free_cell.turned_on = btn->save.Safe_opening_ability;
	btn->sw_q.turned_on = btn->save.q_is_enabled;
    btn->sw_map_realloc.set_tip_msg ("Allows you to open any\ncell if there is no logical\nmove\nMines will change their\npositions if you click on\ncell containing one");
    btn->sw_open_mine_free_cell.set_tip_msg ("Allows you to click special \nbutton to open one of\nmine-free cell if there\nis no logical move");
	btn->sw_q.set_tip_msg ("Marks are used to denote\nsome cells");
#undef DF1
#undef DF2
#undef DF3

	char c_str[100];
	btn->x[0] = 5;
	btn->x[1] = 66;
	btn->x[2] = 124;
	btn->x[3] = 163;
	btn->y[0] = 32;
	btn->y[1] = 64;
	btn->y[2] = 96;
	btn->hwm[0].max_val = 30;
	btn->hwm[1].max_val = 100;
	btn->hwm[2].max_val = 999;
    btn->hwm[0].min_val = 6;
	btn->hwm[1].min_val = 8;
	btn->hwm[2].min_val = 0;
    global_game_parameters = btn->save.Last_mode;
	FOR (i, 3) {
		btn->hwm[i].active = false;
		btn->hwm[i].txt.setFont (btn->font);
		btn->hwm[i].txt.setCharacterSize (17);
		btn->hwm[i].txt.setColor (my_clr_s[color_theme].font);
		btn->hwm[i].pos = v2i(btn->x[i+1] - 8, btn->y[1] + 4);
		switch (i) {
		case 0:
			btn->hwm[i].upd (global_game_parameters.height);
			break;
		case 1:
			btn->hwm[i].upd (global_game_parameters.width);
			break;
		case 2:
			btn->hwm[i].upd (global_game_parameters.mines);
			break;
		}
    }
	btn->save.First = false;
	btn->cells.setPrimitiveType (sf::Quads);
	make_table (btn);

	btn->add_a.init ("assets/textures/add_a.png", 124, 30);
	btn->add_na.init ("assets/textures/add_na.png", 124, 30);
	btn->add.alpha = 0;
	btn->add.pos = v2f(37, 96);
	btn->add.act = &btn->add_a;
	btn->add.spr = &btn->add_na;
    btn->add.tip.setString ("Add these game parameters\nto template list");
    btn->add.tip_using = true;

	read_string ();
	return (char *)btn;
}