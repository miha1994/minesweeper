#include "o_button.h"
#include "o_control.h"
#include "space.h"
#include "save.h"
#include "quad.h"

void make_table (button *btn) {
	sf::VertexArray *v_a = &btn->cells;
	v_a->clear ();
	vec<sf::Text> *txt = &btn->mode_table;
	txt->clear ();
	quad q;
	quad_set_color (q, my_clr_s[color_theme].safe);
	int x[5];
	x[0] = 205;
	x[1] = 266;
	x[2] = 324;
	x[3] = 363;
	x[4] = 395;
	FOR (i, 3) {
		FOR (j, btn->save.Num_of_templates + 4) {
			quad_set_tex_rect (q, 0, 0, x[i+1] - x[i] - 2, 30);
			quad_set_pos (q, v2f (x[i], j*32), v2f(0,0));
			quad_v_a (q, v_a);
		}
	}
	sf::Text t;
	t.setCharacterSize (17);
	t.setColor (my_clr_s[color_theme].font);
	t.setFont (btn->font);
	FOR (j, btn->save.Num_of_templates + 3) {
		t.setString (Tstr (btn->save.templates[j].height));
		t.setPosition (x[1] - t.getGlobalBounds ().width - 8, 32 * (j+1) + 4);
		txt->push_back (t);
	}
	FOR (j, btn->save.Num_of_templates + 3) {
		t.setString (Tstr (btn->save.templates[j].width));
		t.setPosition (x[2] - t.getGlobalBounds ().width - 8, 32 * (j+1) + 4);
		txt->push_back (t);
	}
	FOR (j, btn->save.Num_of_templates + 3) {
		t.setString (Tstr (btn->save.templates[j].mines));
		t.setPosition (x[3] - t.getGlobalBounds ().width - 8, 32 * (j+1) + 4);
		txt->push_back (t);
	}
	btn->res_buttons.clear ();
    btn->play_buttons.clear ();
    btn->del_buttons.clear ();

#define tmp(i)  btn->res_buttons[i]
#define tmp1(i)  tmp(i).spr = &btn->res_na; tmp(i).act = &btn->res_a; tmp(i).alpha = 0; \
    tmp(i).tip_using = true; \
    tmp(i).tip.setString ("Show statistic");

    btn->res_buttons.assign (btn->save.Num_of_templates + 3, sq_button ());
    
	FOR (i, btn->save.Num_of_templates + 3) {
	    tmp1(i);
		if (btn->save.statistic.find (btn->save.templates[i]) == btn->save.statistic.end () || !btn->save.statistic[btn->save.templates[i]].wins_count) {
			tmp(i).is_dead = true;
		}
        tmp(i).pos = v2f (x[0]-20, (i+1) * 32);
	}

#define tmp(i)  btn->play_buttons[i]
#define tmp1(i)  tmp(i).spr = &btn->play_not_active; tmp(i).act = &btn->play_active; tmp(i).alpha = 0; \
    tmp(i).tip_using = true; \
    tmp(i).tip.setString ("Play using these parameters");
	
    btn->play_buttons.assign (btn->save.Num_of_templates + 4, sq_button ());
    tmp1(0);
    tmp(0).pos = v2f (5, 96);
    
	FOR (j, btn->save.Num_of_templates + 3) {
        int i = j + 1;
	    tmp1(i);
        tmp(i).pos = v2f (x[3], (j+1) * 32);
	}
    
#define tmp(i)  btn->del_buttons[i]
#define tmp1(i)  tmp(i).spr = &btn->del_not_active; tmp(i).act = &btn->del_active; tmp(i).alpha = 0; \
    tmp(i).tip_using = true; \
    tmp(i).tip.setString ("Delete this template");

    btn->del_buttons.assign (btn->save.Num_of_templates, sq_button ());

    FOR (j, btn->save.Num_of_templates) {
        tmp1(j);
		tmp(j).pos = v2f (x[3] + 32, (j+4) * 32);
	}
#undef tmp
#undef tmp1
}

O_RENDER (button_render) {
	O_DECL (button, btn);
    window.draw (btn->bg);
	FOR (i, 3) {
		FOR (j, 2) {
			btn->cell.setPosition (btn->x[i], btn->y[j]);
			btn->cell.setSize (v2f(btn->x[i+1] - btn->x[i] - 2, btn->y[j+1] - btn->y[j] - 2));
			window.draw (btn->cell);
		}
	}
	window.draw (btn->cells);
	btn->height.setPosition(208, 5);
	btn->width.setPosition(270, 5);
    window.draw (btn->height);
    window.draw (btn->width);
	btn->height.setPosition(8,37);
	btn->width.setPosition(70,37);
	window.draw (btn->height);
    window.draw (btn->width);
	btn->mine.itself.setPosition(132,34);
	btn->mine.draw (&window);
	btn->mine.itself.setPosition(332,2);
	btn->mine.draw (&window);
	window.draw (btn->Custom_mode);
	FOR (i, 3) {
		window.draw (btn->hwm[i].txt);
		if (btn->hwm[i].active && btn->hwm[i].time < 0.85) {
			btn->line[0].position.x = btn->hwm[i].pos.x + 3;
			btn->line[0].position.y = btn->hwm[i].pos.y;
			btn->line[0].color = my_clr_s[color_theme].font;
			btn->line[1].position.x = btn->hwm[i].pos.x + 3;
			btn->line[1].position.y = btn->hwm[i].pos.y+20;
			btn->line[1].color = my_clr_s[color_theme].font;
			window.draw (btn->line, 2, sf::Lines);
		}
	}
	forvector (p, end, sf::Text, btn->mode_table) {
		window.draw (*p);
	}
	forvector (p, end, sq_button, btn->play_buttons) {
		p->draw (&window);
	}
    forvector (p, end, sq_button, btn->del_buttons) {
		p->draw (&window);
	}
	forstl (p, end, btn->res_buttons) {
		p->draw (&window);
	}
	btn->add.draw (&window);
	btn->crt.render ();
    btn->sw_map_realloc.draw (&window);
    btn->sw_open_mine_free_cell.draw (&window);
	btn->sw_q.draw (&window);
	btn->shadow.draw (&window);
}

O_DEL (button_del) {
	O_DECL (button, btn);
	delete btn;
}

char *button_load (char *parent_space);
O_UPDATE (button_update);

std_obj_functions button_f (button_load, button_update, button_render, button_del);