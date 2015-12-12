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
	window.create (sf::VideoMode (MY_WIND_WIDTH, MY_WIND_HEIGHT), "Minesweeper", sf::Style::Titlebar | sf::Style::Close);
	sf::Image image;
	image.loadFromFile( "assets/textures/icon.png" );
	window.setIcon( image.getSize().x, image.getSize().y, image.getPixelsPtr() );
	window.setVerticalSyncEnabled (true);
	window.setFramerateLimit (60);

    btn->font.loadFromFile ("assets/fonts/cg.ttf");
	btn->mine.init ("assets/textures/mine.png", 25, 25);

    btn->height.setFont (btn->font);
    btn->height.setColor (my_clr_s[color_theme].font);
    btn->height.setString ("Height");
    
    btn->width.setFont (btn->font);
    btn->width.setColor (my_clr_s[color_theme].font);
    btn->width.setString ("Width");

    btn->bg.setPosition (0,0);
    btn->bg.setSize (v2f(440,400));
	btn->bg.setFillColor (my_clr_s[color_theme].bg);

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
    btn->hwm[0].min_val = 2;
	btn->hwm[1].min_val = 8;
	btn->hwm[2].min_val = 0;
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
	get_save_status (&btn->save);
	btn->save.First = false;
	btn->cells.setPrimitiveType (sf::Quads);
	make_table (btn);

	btn->add_a.init ("assets/textures/add_a.png", 124, 30);
	btn->add_na.init ("assets/textures/add_na.png", 124, 30);
	btn->add.alpha = 0;
	btn->add.pos = v2f(37, 96);
	btn->add.act = &btn->add_a;
	btn->add.spr = &btn->add_na;

	read_string ();
	return (char *)btn;
}