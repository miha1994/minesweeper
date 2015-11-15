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
			} else {
				mbutton[i].just_released = true;
			}
		} else {
			mbutton[i].just_pressed = false;
			mbutton[i].just_released = false;
		}
		mbutton[i].pressed_now = new_v;
	}
}