#pragma once
#include "sfml.h"
#include "main_header.h"
#include "quad.h"
#include "tooltip.h"
#include <iostream>

class tooltip;

struct sprite {
	sf::Texture texture;
	sf::Sprite itself;
	void init(const char *file_name, int size_x, int size_y, bool smooth = false);
	void draw (sf::RenderWindow *wnd, bool coords = false, v2f xy = v2f(0,0));
};

#define CLR sf::Color

CLR CLR_BLIND (CLR c);
CLR CLR_ADD (CLR col1, CLR col2);

CLR operator * (CLR col, float m);

struct color_s {
    CLR unknown;
    CLR mine;
    CLR safe;
    CLR font;
	CLR bg;
	CLR flag;
	CLR win_man;
	color_s (CLR Unknown, CLR Mine, CLR Safe, CLR Font, CLR BG, CLR Flag, CLR Win_man) : unknown (Unknown), mine (Mine), safe (Safe), font (Font), bg (BG), flag (Flag), win_man (Win_man)
    {}
};

extern color_s my_clr_s[];
extern CLR dig_colors[10];

#define get_mouse_pos(v2if)		{v2i i2 = sf::Mouse::getPosition (window); v2if.x = i2.x; v2if.y = i2.y;}
#define in_range(dd,minn,maxx)	if(dd < minn) {dd = minn;} else if (dd > maxx) {dd = maxx;}

enum CRT_TYPE {
    CRT_TYPE_INT,
    CRT_TYPE_FLOAT,
    CRT_TYPE_SPRITE,
    CRT_TYPE_TEXT,
	CRT_TYPE_COLOR
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
	int col_cur_comp;
    v2f mouse_pos;
    vec <elem> elements;
    sf::Font font;
    sf::Text info;
public:
    change_real_time ();
    void add_elem (char *ptr, CRT_TYPE type, std::string pref, std::string postf = "");
    void update ();
    void render ();
	int cur;
    std::string out_file_name;
	~change_real_time ();
};

struct sq_button {
	v2f pos;
	bool pressed_lt;
    bool is_available_to_be_pressed;
    bool on_it;
	sprite *spr;
	sprite *act;
	float alpha;
    tooltip tip;
    bool tip_using;
    bool is_dead;
    sq_button () : is_available_to_be_pressed (true), on_it (false), tip_using (false), is_dead (false) {}
	void draw (sf::RenderWindow *wind);
	bool update (float dt, v2f mouse_pos);
};

template <class T>
class text_one_type {
    T inf;
    v2f position;
public:
    sf::Text txt;
    struct {
        int v;
        void set__xy_is_center_point () {v = 0;}
        void set__xy_is_top_left_point () {v = 1;}
        void set__xy_is_top_right_point () {v = 2;}
    } origin_type;
    void set_pos (v2f pos);
    void set_pos (float x, float y) {set_pos (v2f(x,y));}
    void set_inf_value (T val = T ());
    T get_inf_value () {return inf;}
    sf::Text *operator -> () {
        return &txt;
    }
};

template <class T>
void text_one_type <T>::set_pos (v2f pos) {
    position = pos;
    auto r = txt.getGlobalBounds ();
    switch (origin_type.v) {
        case 0: pos -= r.v_size () * 0.5F; break;
        case 1: break;
        case 2: pos.x -= r.width;
    }
    pos.y -= r.height * 0.5;
    txt.setPosition (v2f(v2i(pos)));
}

namespace std {
	string to_string (string s);
};

template <class T>
void text_one_type <T>::set_inf_value (T val) {
    inf = val;
    txt.setString (Tstr (inf));
    v2f pos = position;
    auto r = txt.getGlobalBounds ();
    switch (origin_type.v) {
        case 0: pos -= r.v_size () *0.5F; break;
        case 1: break;
        case 2: pos.x -= r.width;
    }
    pos.y -= r.height * 0.5;
    txt.setPosition (v2f(v2i(pos)));
}

struct mines_moving {
	char *fld_;
	struct mine {
		v2i tmp_goal;
		v2i cur_pos;
		v2i nxt_pos;
		float part_of_step;
		float speed;
        float wait;
	};
	struct splinter {
		quad q;
		float speed;
		float a;
	};
	struct flag {
		v2i start_pos;
		float time;
	};
	struct digit : public flag {
		int dig;
	};
	std::list <mine> e;
	std::list <flag> f;
	std::list <splinter> s;
	std::list <digit> d;
	int h2;
	int w2;
	void init ();
	bool update (float dt);
};

/*
class r_n {
	public:
	int a, b;
	void Norm ();
	r_n (int c = 1, int d = 1) : a (c), b (d) {Norm (); }
	~r_n () {
	}
	r_n operator + (const r_n &n) const {
		r_n rv (a * n.b + n.a * b, b * n.b);
		rv.Norm ();
		return rv;
	}
	r_n operator - (const r_n &n) const {
		r_n rv (a * n.b - n.a * b, b * n.b);
		rv.Norm ();
		return rv;
	}
	r_n operator * (const r_n &n) const {
		r_n rv (a * n.a, b * n.b);
		rv.Norm ();
		return rv;
	}
	r_n operator / (const r_n &n) const {
		r_n rv (a * n.b, b * n.a);
		rv.Norm ();
		return rv;
	}
	bool operator < (const r_n& r) {
		r_n tmp = *this - r;
		return tmp.a < 0;
	}
	bool operator > (const r_n& r) {
		r_n tmp = *this - r;
		return tmp.a > 0;
	}
	bool operator <= (const r_n& r) {
		r_n tmp = *this - r;
		return tmp.a <= 0;
	}
	bool operator >= (const r_n& r) {
		r_n tmp = *this - r;
		return tmp.a >= 0;
	}
	bool operator == (const r_n& r) {
		return a == r.a && b == r.b;
	}
	bool operator != (const r_n& r) {
		return a != r.a || b != r.b;
	}
	
	std::string to_str () const {
		return std::to_string (a) + (b == 1 ? "" : ":" + std::to_string (b));
	}
	void read () {
		std::cout << "enter a / b.   a = ";
		std::cin >> a;
		std::cout << "b = ";
		std::cin >> b;
		Norm ();
	}
};
*/