#pragma once
#include "main_header.h"

enum mouse {
	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE
};

class button_state {
public:
	bool pressed_now;
	bool just_released;
	bool just_pressed;
};

class INPUT {
public:
	button_state mbutton[3];
	void upd ();
	INPUT () {
		FOR (i, 3) {
			mbutton[i].pressed_now = false;
			mbutton[i].just_pressed = false;
			mbutton[i].just_released = false;
		}
	}
};

extern INPUT input;