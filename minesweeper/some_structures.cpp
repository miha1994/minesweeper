#include "some_structures.h"
#include "main_header.h"
#include "input.h"
#include "o_field.h"

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
    color_s (CLR (175,175,175,255), CLR(255,0,0,255), CLR(225,225,225,225), CLR(0,0,0,255), CLR (255, 255, 255, 255), CLR(255,0,0,255), CLR (0, 0, 0, 255))
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
    if (is_dead) {
        return;
    }
	if (alpha < 254) {
		spr->itself.setPosition (v2f(pos));
        spr->itself.setColor (CLR (255,255,255,255-alpha));
		spr->draw (wind);
	}
	if (alpha > 1) {
		act->itself.setPosition (v2f (pos));
		act->itself.setColor (sf::Color (255,255,255, alpha));
		act->draw (wind);
	}
}

bool sq_button::update (float dt, v2f mouse_pos) {
    if (is_dead) {
        return false;
    }
	bool ret = false;
	spr->itself.setPosition (v2f(pos));
    if ((on_it = spr->itself.getGlobalBounds ().contains (mouse_pos)) && is_available_to_be_pressed) {
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
    if (tip_using) {
        tip.setActive (on_it);
        tip.update (dt);
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

int sign (float f) {
	return f > 0 ? 1 : (f < 0 ? -1 : 0);
}

void mines_moving::init () {
	field *fld = (field *) fld_;
    fld->reallocation_time = 100;
	mine m;
	flag fl;
	digit dg;
	fl.time = 0;
	dg.time = 0;
	h2 = HHH / 2;
	w2 = WWW / 2;
	e.clear ();
	f.clear ();
	s.clear ();
	d.clear ();
	FOR_2D (v, WWW, HHH) {
		MK_C (c, v);
		if (c->flags & CELL_FLAGS_MINE) {
			m.cur_pos = v;
            
			if (v.x <= w2) {
				if (v.y <= h2) {
					m.tmp_goal = v2i (w2+1, rand_n (h2/2));
				} else {
					m.tmp_goal = v2i (rand_n (w2/2), h2);
				}
			} else {
				if (v.y <= h2) {
					m.tmp_goal = v2i (w2 + w2/2 + rand_n (WWW - w2 - w2/2), h2+1);
				} else {
					m.tmp_goal = v2i (w2, h2 + h2/2 + rand_n (HHH - h2 - h2/2));
				}
			}
			v2f dif = v2f(m.tmp_goal - m.cur_pos);
			v2f abs_d = v2f(abs(dif.x), abs (dif.y));
			if (abs_d.x > abs_d.y) {
				if (rand1 > 0.35) {
					m.nxt_pos.y = 0;
					m.nxt_pos.x = sign (dif.x);
				} else {
					m.nxt_pos.x = 0;
					m.nxt_pos.y = sign (dif.y);
				}
			} else {
				if (rand1 > 0.35) {
					m.nxt_pos.x = 0;
					m.nxt_pos.y = sign (dif.y);
				} else {
					m.nxt_pos.y = 0;
					m.nxt_pos.x = sign (dif.x);
				}
			}
			m.part_of_step = 0;
			m.speed = 0;
			e.push_back (m);
        }
		if (c->flags & CELL_FLAGS_MARK) {
			fl.start_pos = v;
			f.push_back (fl);
		}
		if (true) {
			splinter sp;
			sp.speed = 0;
			float rest = 1;
			float new_sp, r_sp;
			while (1) {
				new_sp = rand1;
				r_sp = Min (new_sp, rest);

                sp.speed = -rand1 * 3000;
				sp.a = 300.0 * (HHH - v.y) / HHH + rand1 * 200;
				quad_set_color (sp.q, c->flags & CELL_FLAGS_CLOSED ? my_clr_s[color_theme].unknown : my_clr_s[color_theme].safe);
				quad_set_tex_rect (sp.q, sf::Rect <int> ((1 - rest) * PX_96, 0, r_sp * PX_96, PX_96));
				quad_set_pos (sp.q, v2f (fld->shift, fld->vshift) + v2f(v2f(6, 43) + (v2f (v) + v2f(1-rest, 0)) * 32.0f) * PIX, v2f(fld->pix * r_sp,fld->pix),false);
				s.push_back (sp);

				rest -= new_sp;
				if (rest <= 0) {
					break;
				}
			}
		}
		if (!(c->flags & CELL_FLAGS_CLOSED) && c->val) {
			dg.start_pos = v;
			dg.dig = c->val;
			d.push_back (dg);
		}
	}
    forstl (p, end, e) {
        p->wait = rand1 * sqrt(e.size ());
    }
}

bool mines_moving::update (float dt) {
    dt = Min (dt, 0.07);
	field *fld = (field *) fld_;
	fld->mines.clear ();
	v2f v;
	quad q;
	quad_set_tex_rect (q, SFRECTcv (int, 0,0,fld->pix > 48 ? v2f(96,96) : v2f (48, 48)));
	quad_set_color (q, my_clr_s[color_theme].mine);
	forlist_no_inc (p, end, mine, e) {
        if ((p->wait -= dt) > 0) {
            v = v2f(p->cur_pos) + v2f(p->nxt_pos) * p->part_of_step;
		    quad_set_pos (q, v2f (fld->shift, fld->vshift) + v2f(v2f(6, 43) + v*32.0f) * PIX, v2f(fld->pix,fld->pix),false);
		    quad_v_a (q, &fld->mines);
            ++p;
            continue;
        }
		p->speed += 1. * dt;
		p->speed = Min (p->speed, 20.0);
		p->part_of_step += p->speed * dt;
		if (p->part_of_step >= 1) {
			p->part_of_step -= 1;
			p->cur_pos += p->nxt_pos;
			if (p->cur_pos == p->tmp_goal) {
				if (p->cur_pos == v2i (4, HHH - 2)) {
					p->tmp_goal = v2i (3, HHH - 2);
				} else {
					if (p->cur_pos == v2i (3, HHH - 2)) {
						p = e.erase (p);
						if (e.empty ()) {
							return true;
							this->s.clear ();
						}
						continue;
					}
					if (p->cur_pos.x <= w2) {
						if (p->cur_pos.y <= h2) {
							p->tmp_goal = v2i (w2+1, rand_n (h2/2));
						} else {
							p->tmp_goal = v2i (4, HHH - 2);
						}
					} else {
						if (p->cur_pos.y <= h2) {
							p->tmp_goal = v2i (w2 + w2/2 + rand_n (WWW - w2 - w2/2), h2+1);
						} else {
							p->tmp_goal = v2i (w2, h2 + h2/2 + rand_n (HHH - h2 - h2/2));
						}
					}
				}
			}
			v2f dif = v2f(p->tmp_goal - p->cur_pos);
			v2f abs_d = v2f(abs(dif.x), abs (dif.y));
			if (abs_d.x > abs_d.y) {
				if (rand1 > 0.35 || abs_d.y * 3 < abs_d.x) {
					p->nxt_pos.y = 0;
					p->nxt_pos.x = sign (dif.x);
				} else {
					p->nxt_pos.x = 0;
					p->nxt_pos.y = sign (dif.y);
				}
			} else {
				if (rand1 > 0.35 || abs_d.x * 3 < abs_d.y) {
					p->nxt_pos.x = 0;
					p->nxt_pos.y = sign (dif.y);
				} else {
					p->nxt_pos.y = 0;
					p->nxt_pos.x = sign (dif.x);
				}
			}
		}
		v = v2f(p->cur_pos) + v2f(p->nxt_pos) * p->part_of_step;
		quad_set_pos (q, v2f (fld->shift, fld->vshift) + v2f(v2f(6, 43) + v*32.0f) * PIX, v2f(fld->pix,fld->pix),false);
		quad_v_a (q, &fld->mines);
		++p;
	}
	forlist (p, end, splinter, s) {
		p->a += 500 * dt;
		p->speed += p->a * dt;
        if (p->speed > 0) {
		    quad_move (p->q, v2f (0, p->speed * dt));
        }
		quad_v_a (p->q, &fld->cells);
	}
	fld->flags.clear ();
	forlist_no_inc (p, end, flag, f) {
		p->time += dt;
		if (p->time >= 0.97) {
			p = f.erase (p);
			continue;
		}
		CLR clr = my_clr_s[color_theme].flag;
		clr.a = (1 - p->time) * 255;
		quad_set_color (q, clr);
		v = v2f(p->start_pos);
		v.y += p->time;
		quad_set_pos (q, v2f (fld->shift, fld->vshift) + v2f(v2f(6, 43) + v*32.0f) * PIX, v2f(fld->pix,fld->pix),false);
		quad_v_a (q, &fld->flags);
		++p;
	}
	fld->digits.clear ();
	forlist_no_inc (p, end, digit, d) {
		p->time += dt;
		if (p->time >= 0.97) {
			p = d.erase (p);
			continue;
		}
		CLR clr = dig_colors [p->dig];
		clr.a = (1 - p->time) * 255;
		quad_set_color (q, clr);
		v = v2f(p->start_pos);
		v.y += p->time;
		quad_set_tex_rect (q, SFRECTcv (int, p->dig * (fld->pix > 48 ? 96 : 48), 0, fld->pix > 48 ? v2f(96,96) : v2f (48, 48)));
		quad_set_pos (q, v2f (fld->shift, fld->vshift) + v2f(v2f(6, 43) + v*32.0f) * PIX, v2f(fld->pix,fld->pix),false);
		quad_v_a (q, &fld->digits);
		++p;
	}
	return false;
}

namespace std {
	string to_string (string s) {
		return s;
	}
};