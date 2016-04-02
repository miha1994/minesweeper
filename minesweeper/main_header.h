#pragma once

#include "sfml.h"
#include <string>
#include <list>

extern sf::RenderWindow window;
extern char *main_control;
extern int R_WIND_WIDTH;
extern int R_WIND_HEIGHT;
extern float X_FACTOR, Y_FACTOR;
extern bool Global_Bool_Key_Pressed;

#define Min(a,b)	(((a)>(b))?(b):(a))
#define Max(a,b)	(((a)>(b))?(a):(b))

#define FOR(I_,N_)				for (int I_=0; I_<N_; ++I_)
#define For(N_)					FOR (tmp_variable_for_loop__,N_)
#define FOR_2D(v_, M_X, M_Y)	for (v2i v_ = v2i(0,0); v_.x < M_X; ++v_.x) \
	for (v_.y = 0; v_.y < M_Y; ++v_.y)
#define MK_C(c, point)			cell *c = &fld->a[point];
#define POINT_IN_RECT(p, x0, y0, x1, y1)	(p.x >= x0 && p.y >= y0 && p.x < x1 && p.y < y1)

#define TRN_ON(mask, flag)	(mask) |= (flag)
#define TRN_OFF(mask,flag)	(mask) &= (~(flag))

#define forvector(p_,end_,type_,vector_)		for (std::vector<type_>::iterator (p_) = (vector_).begin(), (end_) = (vector_).end(); (p_) != (end_); ++(p_))
#define formap(p_,end_,type1_,type2_,map_)		for (std::map<type1_,type2_>::iterator (p_) = (map_).begin(), (end_) = (map_).end(); (p_) != (end_); ++(p_))
#define forset(p_,end_,type_,set_)				for (std::set<type_>::iterator (p_) = (set_).begin(), (end_) = (set_).end(); (p_) != (end_); ++(p_))
#define forlist(p_,end_,type_,list_)			for (std::list<type_>::iterator (p_) = (list_).begin(), (end_) = (list_).end(); (p_) != (end_); ++(p_))
#define forlist_no_inc(p_,end_,type_,list_)		for (std::list<type_>::iterator (p_) = (list_).begin(), (end_) = (list_).end(); (p_) != (end_); )
#define forstl(p_,end_,container_)              for (auto (p_) = (container_).begin (), (end_) = (container_).end (); (p_) != (end_); ++(p_))
#define forstl_no_inc(p_,end_,container_)       for (auto (p_) = (container_).begin (), (end_) = (container_).end (); (p_) != (end_); )

extern int MY_WIND_WIDTH, MY_WIND_HEIGHT;
extern int SC_WIDTH, SC_HEIGHT;
extern int part;
extern int level;
extern bool mouse_left_pressed;

#define SFRECTcv(Type_,X_,Y_,Size_)    sf::Rect <Type_> (X_, Y_, (Size_).x, (Size_).y)

typedef unsigned int uint;
#define vec		std::vector
#define vec_erase(vector_,I_)	vector_.erase (vector_.begin () + I_)
#define v2i		sf::Vector2i
#define v2f		sf::Vector2f
#define v2u		sf::Vector2u
#define MAP(T)	std::map <std::string, T>
#define kb		sf::Keyboard
#define VS(v)   ((v).x), ((v).y)

#define sleep10			{sf::sleep (sf::milliseconds(10));}

typedef std::pair<std::string, std::string> instruction;
typedef std::list<instruction>	to_do_list;

instruction get_command(std::string cm1);

extern v2i std_directions[];
int vec_to_dir (v2i v); 
float vec_to_pi (v2f vector_);
float length (v2f v);
#define D_UP	0
#define D_RIGHT 1
#define D_DOWN	2
#define D_LEFT	3

extern int error_generator;
#define ERROR   (error_generator = 100 / error_generator)

inline v2i operator ! (v2f vf) {return v2i (vf);}
inline v2f operator ! (v2i vi) {return v2f (vi);}

class in_out_d {
public:
	std::string func_name;
	bool print;
	in_out_d (std::string s);
	~in_out_d ();
};

#define CNTRL(f_name)	in_out_d in_out_d_instance(f_name)

#define rand1	(rand() / float(RAND_MAX))

int rand_n (int n);

class sound {
	int p;
public:
	sf::SoundBuffer buf;
	sf::Sound snd[3];
	void loadFromFile (const std::string &file_name, float vol = 1.0f) {
		buf.loadFromFile (file_name);
		FOR(z,3) {
			snd[z].setBuffer (buf);
			snd[z].setVolume (vol);
		}
		p = 0;
	}
	void play (bool rand_pitch = false, float min_pitch=0.0f, float max_pitch = 0.0f) {
		if (rand_pitch) {
			snd[p].setPitch (min_pitch + rand1 * (max_pitch - min_pitch));
		}
		snd[p++].play ();
		if (p > 2) {
			p = 0;
		}
	}
	void setVolume (float new_vol) {
		FOR (z, 3) {
			snd[z].setVolume (new_vol);
		}
	}
	void stop () {
		FOR (z, 3) {
			snd[z].stop ();
		}
	}
	bool isPlaying () {
		FOR (z, 3) {
			if (snd[z].getStatus () == sf::Sound::Playing) {
				return true;
			}
		}
		return false;
	}
};

#define Tstr(something)		std::to_string (something)

struct game_parameters {
	int height;
	int width;
	int mines;
	game_parameters (int h, int w, int m) : height (h), width (w), mines (m) {}
	game_parameters () {}
};

extern int color_theme;
extern game_parameters global_game_parameters;


#define PIX  (float (fld->pix / 32.0))
#define PX_1	(int(fld->pix > 48))
#define PX_96	(fld->pix > 48 ? 96 : 48)