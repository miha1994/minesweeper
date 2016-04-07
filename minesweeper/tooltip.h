#pragma once
#include <string>
#include "main_header.h"

/*
________________________________________

 tooltip
________________________________________
*/

class tooltip {
    static sf::Font font;
    static bool font_is_ready;
    static int count_of_active;
    float time;
    bool active;
    sf::RenderWindow wind;
    sf::Text msg;
    tooltip (const tooltip &) { init (""); }
    const tooltip & operator = (const tooltip &) const {return *this;}
    void init (std::string message = "");
public:
    float delay_time;
    tooltip (std::string message = "") {init (message);}
    void setString (std::string message);
    void activate ();
    void update (float dt);
    void deactivate ();
    void setActive (bool act) {if (act) activate (); else deactivate ();}
	~tooltip () {deactivate ();}

    friend struct sq_button;
};


/*
________________________________________

 option_switcher
________________________________________
*/

class sprite;

class option_switcher {
    v2f pos;
    v2f size;
    sf::Text txt;
    bool ready;
    sprite *spr_not_active;
    sprite *spr_active;
    tooltip tip;
    bool tip_using;
    void zero_init () {ready = false;}
public:
    bool turned_on;
    option_switcher ( const option_switcher& other ) {zero_init ();}
    option_switcher& operator=( const option_switcher& ) {zero_init ();}
    option_switcher () {zero_init ();}
    void init (std::string option, sf::Font *fnt, v2f pos_, v2f size_, sprite *spr_not_active_, sprite *spr_active_);
    bool update (float dt, sf::RenderWindow *wind);
    void draw (sf::RenderWindow *wind);
    void set_tip_msg (std::string);
    sf::Text &get_text () {
        if (!ready) {
            ERROR; // option_switcher can't be used without initializing
        }
        return txt;
    }
};