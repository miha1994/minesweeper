#include "table_stat.h"
#include "some_structures.h"

table_wind records;


extern bool no_pause;
extern sf::RenderWindow *rwind;


bool operator < (const round_results & r1, const round_results & r2) {
	return r1.time < r2.time || (r1.time == r2.time && r1.hints_used < r2.hints_used);
}

void table_wind::close () {if (active && wind.isOpen ()) { no_pause = true; wind.close (); } }

const int t_width = 590;
const int t_height = 450;
const int char_size = 20;
const int place_width = 53;
const int place_x = 328;
const int time_width = 83;
const int time_x = place_x + time_width;
const int hints_used_width = 104;
const int hints_used_x = time_x + hints_used_width;
const int y_step = 32;
const int y_shift = 90;
const int gray_intensity = 230;
const int info_x = 150;
const int info_y = 80;
const CLR gray (gray_intensity, gray_intensity, gray_intensity, 255);

int ds_y = 10;
int ds_place = -30;
int ds_time = -89;
int ds_hint = -78;

void table_wind::show (const table_stat &ts, float ttime, int realloc_count) {
	no_pause = true;
	if (!active) {
		active = true;
	} else {
		if (wind.isOpen ()) {
			wind.close ();
		}
	}
	wind.create (sf::VideoMode(t_width, t_height), "Results", sf::Style::Titlebar | sf::Style::Close);
	wind.setPosition (sf::Mouse::getPosition () + v2i (50,50));
	wind.clear (CLR (170,195,190,255));

	sprite spr;
	spr.init ("assets/textures/pixel.png", 381, 369);
	spr.itself.setPosition (0, 170);
	spr.itself.setColor (CLR(255,255, 255, 140));
	spr.draw (&wind);

	sf::RectangleShape rs;
	rs.setPosition (0, 70);
	rs.setSize (v2f (217, 100));
	rs.setFillColor (CLR (160, 180, 120, 255));
	wind.draw (rs);

	rs.setPosition (270, 70);
	rs.setSize (v2f (500, 500));
	rs.setFillColor (CLR (200,200,180,155));
	wind.draw (rs);

	rs.setPosition (70, 10);
	rs.setSize (v2f (t_width - 2*70, 50));
	rs.setFillColor (CLR (220,220,180,255));
	wind.draw (rs);

	rs.setPosition (10, 10);
	rs.setSize (v2f (t_width - 20, t_height - 20));
	rs.setFillColor (CLR (200,200,255,0));
	rs.setOutlineColor (CLR (0,50,0,255));
	rs.setOutlineThickness (10);
	wind.draw (rs);

	sf::VertexArray va;
	va.setPrimitiveType (sf::Quads);
	quad q;
	quad_set_color (q, gray);
	
	auto p = ts.wins.begin ();
	bool was_red = realloc_count < 0;
	FOR (i, ts.wins.size () + 1) {
		if ((i && p->time == ttime && p->hints_used == realloc_count || i == 10) && !was_red) {
			quad_set_color (q, CLR (200,50,50,255));
			was_red = true;
		}
		int y = y_shift + y_step * i - ds_y;

		quad_set_pos (q, v2f(place_x + 1 + -30, y), v2f (50 - 1, y_step - 2), 0);
		quad_v_a (q, &va);

		quad_set_pos (q, v2f(place_x + 110 + 1 + -89, y), v2f (110 - 1, y_step - 2), 0);
		quad_v_a (q, &va);

		quad_set_pos (q, v2f(place_x + 210 + 1 + -78, y), v2f (100 - 1, y_step - 2), 0);
		quad_v_a (q, &va);
		if (i && p->time == ttime && p->hints_used == realloc_count) {
			quad_set_color (q, gray);
		}
		if (i) {++p;}
		if (i == 10) {
			break;
		}
	}
	
	wind.draw (va);

	sf::Font font;
	font.loadFromFile ("assets/fonts/consolas.ttf");

	text_one_type <std::string> txt;
	char str[40];
	txt->setFont (font);
	txt->setCharacterSize (char_size);
	txt->setColor (CLR::Black);
	txt.origin_type.set__xy_is_top_right_point ();

	int i = 0;
	was_red = realloc_count < 0;
	forset (p, end, round_results, ts.wins) {
		int y = y_shift + y_step * (i+1);

		if (p->time == ttime && p->hints_used == realloc_count) {
			was_red = true;
		}

		if (i == 9 && !was_red) {
			auto nxt = p;
			while ( (++(nxt = p)) != end && ! (p->time == ttime && p->hints_used == realloc_count)) {
				p = nxt;
				++i;
			}
		}

		txt.set_pos (place_x, y);
		sprintf (str, "%d", i+1);
		txt.set_inf_value (str);
		wind.draw (txt.txt);

		txt.set_pos (time_x, y);
		sprintf (str, "%d.", int (p->time));
		txt.set_inf_value (str);
		wind.draw (txt.txt);

		txt.set_pos (time_x + 2, y);
		int g = int((p->time - int(p->time)) * 1000);
		sprintf (str, "%d%d%d", g / 100, (g / 10) % 10, g % 10);
		txt.origin_type.set__xy_is_top_left_point ();
		txt.set_inf_value (str);
		wind.draw (txt.txt);
		txt.origin_type.set__xy_is_top_right_point ();

		txt.set_pos (hints_used_x, y);
		sprintf (str, "%d", p->hints_used);
		txt.set_inf_value (str);
		wind.draw (txt.txt);

		++i;
		if (i >= 10) {
			break;
		}
	}

	int y = y_shift + 5;

	txt.origin_type.set__xy_is_center_point ();

	txt.set_pos (place_x - 7, y);
	txt.set_inf_value ("pos");
	wind.draw (txt.txt);

	txt.set_pos (time_x - 8, y);
	txt.set_inf_value ("time");
	wind.draw (txt.txt);

	txt.set_pos (hints_used_x - 5, y);
	txt.set_inf_value ("hints");
	wind.draw (txt.txt);


	txt->setColor (CLR (5, 20, 20, 255));

	int wins = ts.wins_count;

	txt.origin_type.set__xy_is_top_right_point ();
	sprintf (str, "total wins :");
	txt.set_inf_value (str);
	txt.set_pos (info_x, info_y);
	wind.draw (txt.txt);
	txt.origin_type.set__xy_is_top_left_point ();
	sprintf (str, "%d", wins);
	txt.set_inf_value (str);
	txt.set_pos (info_x+10, info_y);
	wind.draw (txt.txt);

	txt.origin_type.set__xy_is_top_right_point ();
	sprintf (str, "total fails :");
	txt.set_inf_value (str);
	txt.set_pos (info_x, info_y + y_step);
	wind.draw (txt.txt);
	txt.origin_type.set__xy_is_top_left_point ();
	sprintf (str, "%d", ts.fails_count);
	txt.set_inf_value (str);
	txt.set_pos (info_x + 10, info_y + y_step);
	wind.draw (txt.txt);

	txt.origin_type.set__xy_is_top_right_point ();
	sprintf (str, "percentage :");
	txt.set_inf_value (str);
	txt.set_pos (info_x, info_y + 2*y_step);
	wind.draw (txt.txt);
	txt.origin_type.set__xy_is_top_left_point (); // 
	sprintf (str, "%d %%", int(double(wins * 100) / (ts.fails_count + wins)));
	txt.set_inf_value (str);
	txt.set_pos (info_x + 10, info_y + 2*y_step);
	wind.draw (txt.txt);

	txt.origin_type.set__xy_is_center_point ();
	txt->setCharacterSize (50);
	txt.set_inf_value ("R E S U L T S");
	txt->setColor (CLR(50,50,50,255));
	txt.set_pos (t_width / 2 - 10, 30);
	wind.draw (txt.txt);

    wind.display ();
	rwind = &wind;
}

void table_wind::update (float dt) {
	if (!active || !wind.isOpen ()) {
		return;
	}
    sf::Event event;
    while (wind.pollEvent(event))
    {
		switch (event.type) {
        case sf::Event::Closed:
			no_pause = true;
            wind.close();
			break;
		}
	}
}