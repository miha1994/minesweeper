#include "some_structures.h"
#include "main_header.h"
#include "input.h"

void sprite::init(const char *file_name, int size_x, int size_y, bool smooth) {
	texture.loadFromFile (file_name);
	texture.setSmooth (smooth);
	itself.setTexture (texture);
	itself.setTextureRect (sf::Rect<int> (0,0,size_x,size_y));
}

void sprite::draw (sf::RenderWindow *wnd, bool f, v2f xy) {
	if (f) {
		itself.setPosition (xy);
	}
	wnd->draw (itself);
}

color_s my_clr_s[] = {
    color_s (CLR (175,175,175,255), CLR(255,0,0,255), CLR(225,225,225,225), CLR(0,0,0,255), CLR (255, 255, 255, 255), CLR(255,0,0,255))
};
CLR dig_colors[10];

change_real_time::change_real_time () {
    cur = 0;
    mouse_pos = v2f(0,0);
	elements.clear ();
    out_file_name = "crt_output.txt";
    font.loadFromFile ("assets/fonts/cg.ttf");
	col_cur_comp = 0;
    info.setFont (font);
    info.setColor (sf::Color::Black);
    info.setCharacterSize (20);
    info.setPosition (0,0);
}

void change_real_time::add_elem (char *ptr, CRT_TYPE type, std::string pref, std::string postf) {
    change_real_time::elem new_el;
    new_el.ptr = ptr;
    new_el.type = type;
    new_el.delta = 0;
	new_el.pref = pref;
	new_el.postf = postf;
    switch (type) {
    case CRT_TYPE_INT:
        new_el.v0 = *((int *)ptr);
        break;
    case CRT_TYPE_FLOAT:
        new_el.v0 = *((float *)ptr);
        break;
    }
    elements.push_back(new_el);
}

void change_real_time::update () {
    FOR (i, 10) {
        if (kb::isKeyPressed (kb::Key (int(kb::Num0) + i))) {
            cur = i;
            break;
        }
    }
    v2f m;
    get_mouse_pos (m);
    if (cur >= 0 && cur < elements.size ()) {
        elem *e = &elements[cur];
        switch (e->type) {
        case CRT_TYPE_INT:
        case CRT_TYPE_FLOAT:
            {
				float dx = (m.x - mouse_pos.x);
				if (kb::isKeyPressed (kb::Q)) {
					dx *= 0.5;
				}
				if (kb::isKeyPressed (kb::W)) {
					dx *= 0.4;
				}
				if (kb::isKeyPressed (kb::E)) {
					dx *= 0.3;
				}
                e->delta += dx;
                float res = e->v0 + e->delta;
                if (e->type == CRT_TYPE_INT) {
                    *((int *)(e->ptr)) = res;
                } else {
                    *((float *)(e->ptr)) = res;
                }
            }
            break;
		case CRT_TYPE_COLOR:
			{
				float dx = (m.x - mouse_pos.x);
				CLR *c = (CLR *) e->ptr;
				if (kb::isKeyPressed (kb::Q)) {
					col_cur_comp = 0;
					e->v0 = c->r;
					e->delta = 0;
				}
				if (kb::isKeyPressed (kb::W)) {
					col_cur_comp = 1;
					e->v0 = c->g;
					e->delta = 0;
				}
				if (kb::isKeyPressed (kb::E)) {
					col_cur_comp = 2;
					e->v0 = c->b;
					e->delta = 0;
				}
				if (kb::isKeyPressed (kb::R)) {
					col_cur_comp = 3;
					e->v0 = c->a;
					e->delta = 0;
				}
				if (kb::isKeyPressed (kb::T)) {
					col_cur_comp = 4;
					e->delta = 0;
				}
				e->delta += dx * 0.3f;
				if (dx) {
					float t;
					t = e->v0 + e->delta;
					in_range (t, 0, 255);
					switch (col_cur_comp) {
					case 0:
						c->r = t;
						break;
					case 1:
						c->g = t;
						break;
					case 2:
						c->b = t;
						break;
					case 3:
						c->a = t;
						break;
					}
				}
			}
			break;
        case CRT_TYPE_SPRITE:
            {
                sprite *spr = (sprite *)e->ptr;
                spr->itself.setPosition (m);
            }
            break;
        case CRT_TYPE_TEXT:
            {
                sf::Text *txt = (sf::Text *)e->ptr;
                txt->setPosition (m);
            }
            break;
        }
        mouse_pos = m;
    } else {
        info.setPosition (m);
    }
}

void change_real_time::render () {
    if (cur >= 0 && cur < elements.size ()) {
        elem *e = &elements[cur];
        switch (e->type) {
        case CRT_TYPE_INT:
        case CRT_TYPE_FLOAT:
            {
                float res = (e->type == CRT_TYPE_INT ? *((int *)(e->ptr)) : *((float *)(e->ptr)));
                info.setString (std::to_string (res));
            }
            break;
		case CRT_TYPE_COLOR:
			{
				CLR *c = (CLR *) e->ptr;
				std::string res = "";
				res += std::to_string (c->r) + " ";
				res += std::to_string (c->g) + " ";
				res += std::to_string (c->b) + " ";
				res += std::to_string (c->a);
				info.setString (res);
			}
			break;
        case CRT_TYPE_SPRITE:
            {
				char str[40];
				v2f pos = ((sprite *)e->ptr)->itself.getPosition ();
				sprintf(str, "%g %g", pos.x, pos.y);
				info.setString (str);
            }
            break;
        case CRT_TYPE_TEXT:
            {
				char str[40];
				v2f pos = ((sf::Text *)e->ptr)->getPosition ();
				sprintf(str, "%g %g", pos.x, pos.y);
				info.setString (str);
            }
            break;
        }
        window.draw (info);
    }
}

change_real_time::~change_real_time() {
    FILE *out = fopen (out_file_name.c_str (), "w");
    int count = 0;
    forvector (p, end, elem, elements) {
        switch (p->type) {
        case CRT_TYPE_INT:
			fprintf(out, "%s%d%s\n", p->pref.c_str (), *((int *)p->ptr), p->postf.c_str ());
            break;
        case CRT_TYPE_FLOAT:
			fprintf (out, "%s%g%s\n", p->pref.c_str (), *((float *)p->ptr), p->postf.c_str ());
            break;
		case CRT_TYPE_COLOR:
			{
				CLR *c = (CLR *) p->ptr;
				fprintf (out, "%s = CLR(%d,%d,%d,%d);\n", p->pref.c_str (), c->r, c->g, c->b, c->a);
			}
			break;
        case CRT_TYPE_SPRITE:
            {
                v2f c = ((sprite *)p->ptr)->itself.getPosition ();
				fprintf(out, "%sitself.setPosition(%g,%g);\n", p->pref.c_str (), c.x, c.y);
            }
            break;
        case CRT_TYPE_TEXT:
            {
                v2f c = ((sf::Text *)p->ptr)->getPosition ();
                fprintf(out, "%ssetPosition(%g,%g);\n", p->pref.c_str (), c.x, c.y);
            }
            break;
        }
        ++count;
    }
    fclose (out);
}

void sq_button::draw (sf::RenderWindow *wind) {
	if (alpha < 254) {
		spr->itself.setPosition (v2f(pos));
		spr->draw (wind);
	}
	if (alpha > 1) {
		act->itself.setPosition (v2f (pos));
		act->itself.setColor (sf::Color (255,255,255, alpha));
		act->draw (wind);
	}
}

bool sq_button::update (float dt, v2f mouse_pos) {
	bool ret = false;
	spr->itself.setPosition (v2f(pos));
	if (spr->itself.getGlobalBounds ().contains (mouse_pos)) {
		alpha += dt * 1255;
		alpha = Min (255, alpha);
		if (input.mbutton[MOUSE_LEFT].just_released) {
			if (!pressed_lt) {
				ret = true;
			}
			pressed_lt = true;
		} else {
			pressed_lt = false;
		}
	} else {
		alpha -= dt * 1255;
		alpha = Max (0, alpha);
		pressed_lt = false;
	}
	return ret;
}

void clr_norm (CLR *col) {
	in_range (col->a, 0, 255);
	in_range (col->r, 0, 255);
	in_range (col->g, 0, 255);
	in_range (col->b, 0, 255);
}

CLR operator * (CLR col, float m) {
	col.b *= m;
	col.r *= m;
	col.g *= m;
	col.a *= m;
	clr_norm (&col);
	return col;
}
CLR CLR_BLIND (CLR c) {
	c.a = 0;
	return c;
}
CLR CLR_ADD (CLR col1, CLR col2) {
	col1.r += col2.r;
	col1.g += col2.g;
	col1.b += col2.b;
	col1.a += col2.a;
	clr_norm (&col1);
	return col1;
}
/*
void r_n::Norm () {
	int mx = std::max (sqrt (abs (a)), sqrt (abs (b)));
	for (int i = 2; i <= mx; ++i) {
		while (a % i == 0 && b % i == 0) {
			a /= i;
			b /= i;
		}
	}
	if (b < 0) {
		b = -b;
		a = -a;
	}
	if (a == 0) {
		b = 1;
	}
	if (b != 1) {
		exit (0);
	}
}
*/