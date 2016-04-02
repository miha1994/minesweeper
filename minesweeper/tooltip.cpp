#include "tooltip.h"
#include "some_structures.h"
#include "input.h"

sf::Font tooltip::font;
bool tooltip::font_is_ready = false;
int tooltip::count_of_active = 0;

extern bool no_pause;

void tooltip::init (std::string message) {
    active = false;
    if (!font_is_ready) {
        font_is_ready = true;
        font.loadFromFile ("assets/fonts/cg.ttf");
    }
    msg.setString (message);
    msg.setFont (font);
    msg.setColor (CLR (60,60,60,255));
    msg.setCharacterSize (15);
    time = 0;
    delay_time = 1;
}
void tooltip::setString (std::string message) {
    msg.setString (message);
}
void tooltip::activate () {
    if (!active) {
        time = delay_time;
        ++count_of_active;
        active = true;
    }
    if (count_of_active) {
        no_pause = true;
    }
}

void tooltip::update (float dt) {
    if (active) {
        if ((time -= dt) < 0) {
            if (!wind.isOpen ()) {
                auto r = msg.getGlobalBounds ();
                v2i sz (r.width+6, r.height + 12);
                msg.setPosition (3,3);
                wind.create (sf::VideoMode(VS(sz)), "", sf::Style::None);
                wind.clear (CLR (255,255,255,128));
                wind.draw (msg);
                sf::RectangleShape rs;
                rs.setSize (v2f(sz - v2i(2,2)));
                rs.setFillColor (CLR_BLIND (CLR ()));
                rs.setOutlineColor (CLR (60,60,60,255));
                rs.setOutlineThickness (10);
                rs.setPosition (1,1);
                wind.draw (rs);
                wind.setPosition (sf::Mouse::getPosition () + v2i (0, 19));
                wind.display ();
            }
        }
    }
}

void tooltip::deactivate () {
    if (active) {
        if (wind.isOpen ()) {
            wind.close ();
        }
        active = false;
        --count_of_active;
    }
    if (!count_of_active) {
        no_pause = false;
    }
}



void option_switcher::init (std::string option, sf::Font *fnt, v2f pos_, v2f size_, sprite *spr_not_active_, sprite *spr_active_) {
    if (ready) {
        ERROR; // option_switcher can be initialized only once!!!
    }
    ready = true;
    pos = pos_;
    size = size_;
    txt.setFont (*fnt);
    txt.setString (option);
    txt.setPosition (pos);
    txt.setColor (CLR::Black);
    spr_not_active = spr_not_active_;
    spr_active = spr_active_;
    turned_on = false;
    tip_using = false;
}

bool option_switcher::update (float dt, sf::RenderWindow *wind) {
    bool rv = false;
    if (!ready) {
        ERROR; // option_switcher can't be used without initializing
    }
    bool containes = sf::Rect <float> (pos, size).contains (!sf::Mouse::getPosition (*wind));
    if (containes && input.mbutton[MOUSE_LEFT].just_released) {
        turned_on = !turned_on;
        rv = true;
    }
    if (tip_using) {
        tip.setActive (containes);
        tip.update (dt);
    }
    return rv;
}

void option_switcher::set_tip_msg (std::string msg) {
    if (!ready) {
        ERROR; // option_switcher can't be used without initializing
    }
    tip.setString (msg);
    tip_using = true;
}

void option_switcher::draw (sf::RenderWindow *wind) {
    if (!ready) {
        ERROR; // option_switcher can't be used without initializing
    }
    sprite * spr = turned_on ? spr_active : spr_not_active;
    v2f sm_s (size);
    sm_s.x = size.y;

    auto r = spr->itself.getGlobalBounds ();
    v2f delta = sm_s - r.v_size ();
    wind->draw (txt);
    spr->itself.setPosition (pos + size - sm_s + delta / 2.0f);
    spr->draw (wind);
}