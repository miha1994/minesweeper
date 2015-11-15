#include "Game.h"
#include "func_pointers.h"
#include "main_header.h"
#include "space.h"
#include "save.h"
#include "input.h"

bool Game::init (bool fullscreen) {
	MY_WIND_WIDTH = 10;//f_modes[0].width;
	MY_WIND_HEIGHT = 10;//f_modes[0].height;

	window.create (sf::VideoMode (MY_WIND_WIDTH, MY_WIND_HEIGHT), "Minesweeper", sf::Style::Default);
	window.setVerticalSyncEnabled (true);
	window.setFramerateLimit (60);
	min_type_info["CONTROL"]	= my_type_info (0, 0);
	min_type_info["FIELD"]		= my_type_info (1, 1);
	min_type_info["BUTTON"]		= my_type_info (2, 2);

	SV_info si;
	get_save_status (&si);
	color_theme = si.Col_theme;
	global_game_parameters = si.Last_mode;
	all_spaces["MAIN"] = load ("assets/inf/main.txt");
	running = true;
	mouse_left_pressed = 0;
	return true;
}

void Game::update () {
	CNTRL ("game_update");
	rand ();
	if (!window.isOpen ()) {
		running = false;
		return;
	}
    sf::Event event;
	Global_Bool_Key_Pressed = false;
    while (window.pollEvent(event))
    {
		switch (event.type) {
        case sf::Event::Closed:
            window.close();
			running = false;
			break;
		case sf::Event::KeyPressed:
			Global_Bool_Key_Pressed = true;
		default:
			break;
		}
	}
	input.upd ();

	float dt = (clock.restart ()).asSeconds ();
	if (dt > 1) {
		dt = 0;
	}
	if (all_spaces["MAIN"]->update (dt)) {
		running = false;
	}
	if (kb::isKeyPressed (kb::Escape)) {
		running = false;
	}
}

#include "quad.h"

void Game::render () {
	CNTRL ("game_render");
	window.clear ();

	all_spaces["MAIN"]->render ();

	window.display ();
}

void Game::del () {
	CNTRL ("game_del");
	formap(p,end,std::string,space*,all_spaces) {
		p->second->del ();
		delete p->second;
	}
	window.close ();
}