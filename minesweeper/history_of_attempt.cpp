#include "history_of_attempt.h"

#define SZ1				sizeof (int), 1, f
#define write(f, what)	fwrite (&(what), sizeof(what), 1, f)
#define read(f, what)	fread (&(what), sizeof(what), 1, f)

#include <ctime>
#include <iostream>
using namespace std;

#define tmp(y,t)  + std::string (#y + Tstr (t))

void history::store () {
	time_t t = time(0);
    struct tm * now = localtime( & t );
#define df(mon)  mon > 9 ? Tstr(mon) : "0" + Tstr (mon)
	int mon = now->tm_mon + 1;
	string mon_s = df(mon);
	string day = df (now->tm_mday);
	string hour = df (now->tm_hour);
	string min_ = df (now->tm_min);
	string sec = df (now->tm_sec);
	FILE *f = fopen (("assets/inf/HS"
		tmp (., now->tm_year + 1900)
		tmp (., mon_s)
		tmp (., day)
		tmp (._, hour)
		tmp (_, min_)
		tmp (_, sec)
		).c_str (), "wb");
	int n = data.size ();
	write (f, n);
	forstl (p, end, data) {
		write (f, p->type);
		write (f, p->point.x);
		write (f, p->point.y);
	}
	fclose (f);
}

#undef tmp

void history::load () {
	FILE *f = fopen ("assets/inf/HS", "rb");
	if (!f) {
		return ;
	}
	int n;
	read (f, n);
	data.assign (n, atom ());
	forstl (p, end, data) {
		read (f, p->type);
		read (f, p->point.x);
		read (f, p->point.y);
	}
	fclose (f);
}

void history::push_rand_n (int rnd) {
	atom a (HISTORY_ATOM_RAND_N_RET_VAL, rnd, rnd);
	data.push_back (a);
}

void history::push_right_click (v2i pos) {
	data.push_back (atom (HISTORY_ATOM_RIGHT_CLICK, VS (pos)));
}

void history::push_left_click (v2i pos) {
	data.push_back (atom (HISTORY_ATOM_LEFT_CLICK, VS (pos)));
}

void history::push_double_click (v2i pos) {
	data.push_back (atom (HISTORY_ATOM_DOUBLE_CLICK, VS (pos)));
}

int history::get_rand_n (int n) {
	if (empty ()) {
		return rand_n (n);
	}
	atom a = data.front ();
	data.erase (data.begin ());
	if (a.type != HISTORY_ATOM_RAND_N_RET_VAL) {
		ERROR;
	}
	return a.point.x;
}

bool history::is_next_click_right () {
	return data.front ().type == HISTORY_ATOM_RIGHT_CLICK;
}

bool history::is_next_click_left () {
	return data.front ().type == HISTORY_ATOM_LEFT_CLICK;
}

bool history::is_next_click_double () {
	return data.front ().type == HISTORY_ATOM_DOUBLE_CLICK;
}

v2i history::get_next_point () {
	atom a = data.front ();
	data.erase (data.begin ());
	if (a.type == HISTORY_ATOM_RAND_N_RET_VAL) {
		ERROR;
	}
	return a.point;
}