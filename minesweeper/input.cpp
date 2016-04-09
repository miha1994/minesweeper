#include "input.h"

INPUT input;

sf::Mouse::Button b_ar[3] = {sf::Mouse::Left, sf::Mouse::Right, sf::Mouse::Middle};

void INPUT::upd () {
	bool new_v;
	FOR (i, 3) {
		new_v = sf::Mouse::isButtonPressed (b_ar[i]);
		if (new_v != mbutton[i].pressed_now) {
			if (new_v) {
				mbutton[i].just_pressed = true;
				mbutton[i].just_released = false;
			} else {
				mbutton[i].just_released = true;
				mbutton[i].just_pressed = false;
			}
		} else {
			mbutton[i].just_pressed = false;
			mbutton[i].just_released = false;
		}
		mbutton[i].pressed_now = new_v;
	}
    FOR (i, 26) {
        new_v = kb::isKeyPressed (kb::Key (i));
        if (new_v != kb_abc[i].pressed_now) {
			if (new_v) {
				kb_abc[i].just_pressed = true;
				kb_abc[i].just_released = false;
			} else {
				kb_abc[i].just_released = true;
				kb_abc[i].just_pressed = false;
			}
		} else {
			kb_abc[i].just_pressed = false;
			kb_abc[i].just_released = false;
		}
		kb_abc[i].pressed_now = new_v;
	}
}