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

	SC_WIDTH = sf::VideoMode::getDesktopMode ().width;
	SC_HEIGHT = sf::VideoMode::getDesktopMode ().height;
	SV_info si;
	get_save_status (&si);
	color_theme = si.Col_theme;
	global_game_parameters = si.Last_mode;
	all_spaces["MAIN"] = load ("assets/inf/main.txt");
	running = true;
	paused = false;
	mouse_left_pressed = 0;
	return true;
}

void Game::update () {
	CNTRL ("game_update");
	//rand ();
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
		case sf::Event::GainedFocus:
			paused = false;
			break;
		case sf::Event::LostFocus:
			paused = true;
			break;
		case sf::Event::Resized:
			R_WIND_WIDTH =
			window.getSize ().x;
			R_WIND_HEIGHT =
			window.getSize ().y;
			break;
		default:
			break;
		}
	}
	input.upd ();

	if (!paused) {
        float dt = (clock.restart ()).asSeconds ();
		if (all_spaces["MAIN"]->update (dt > 1 ? 1 : dt)) {
			running = false;
		}
		if (kb::isKeyPressed (kb::Escape)) {
			running = false;
		}
	}
}

#include "quad.h"

#include "some_structures.h"

void Game::render () {
	CNTRL ("game_render");
	window.clear (my_clr_s[color_theme].safe);

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