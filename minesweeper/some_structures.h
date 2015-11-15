#pragma once
#include "sfml.h"
#include "main_header.h"

struct sprite {
	sf::Texture texture;
	sf::Sprite itself;
	void init(char *file_name, int size_x, int size_y);
	void draw (sf::RenderWindow *wnd, bool coords = false, v2f xy = v2f(0,0));
};

#define CLR sf::Color

CLR operator * (CLR col, float m);

struct color_s {
    CLR unknown;
    CLR mine;
    CLR safe;
    CLR font;
	CLR bg;
    color_s (CLR Unknown, CLR Mine, CLR Safe, CLR Font, CLR BG) : unknown (Unknown), mine (Mine), safe (Safe), font (Font), bg (BG)
    {}
};

extern color_s my_clr_s[];

#define get_mouse_pos(v2if)		{v2i i2 = sf::Mouse::getPosition (window); v2if.x = i2.x; v2if.y = i2.y;}

enum CRT_TYPE {
    CRT_TYPE_INT,
    CRT_TYPE_FLOAT,
    CRT_TYPE_SPRITE,
    CRT_TYPE_TEXT
};

class change_real_time {
    struct elem {
        char *ptr;
        CRT_TYPE type;
        float v0;
        float delta;
		std::string pref;
		std::string postf;
    };
    int cur;
    v2f mouse_pos;
    vec <elem> elements;
    sf::Font font;
    sf::Text info;
public:
    change_real_time ();
    void add_elem (char *ptr, CRT_TYPE type, std::string pref, std::string postf = "");
    void update ();
    void render ();
    std::string out_file_name;
	~change_real_time ();
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