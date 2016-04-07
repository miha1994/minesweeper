#include "field_utility.h"
#include "slau.h"

#define fld this

extern v2i dr[8];
#define FOR_NEIGHBORS(r, Center_)	v2i r = Center_ + dr[0]; for (int z = 0; z < 8; ++z, r = Center_ + dr[z])

struct cl_info {
    v2i coords;
    int num_of_dig_nbrs;
    cl_info (v2i coord, int nbrs) : coords (coord), num_of_dig_nbrs (nbrs) {}
};
bool operator < (const cl_info &c1, const cl_info &c2) {return c1.num_of_dig_nbrs < c2.num_of_dig_nbrs;}

void field::use_hint () {
    if (fld->state != FIELD_STATE_IN_GAME || fld->empty || fld->win () || fld->game_over) {
        return ;
    }
    if (fld->no_moves) {
        std::set <cl_info> q_cells;
        int count;
        FOR_2D (v, WWW, HHH) {
            MK_C (c, v);
            if (c->flags & CELL_FLAGS_CLOSED && c->flags & CELL_Q && !(c->flags & CELL_FLAGS_MINE)) {
                count = 0;
                FOR_NEIGHBORS (r, v) {
                    MK_C (c1, r);
                    if (!(c1->flags & CELL_FLAGS_CLOSED)) {
                        FOR_NEIGHBORS (p, r) {
                            MK_C (c, p);
                            if (c->flags & CELL_P_MINE) {
                                count--;
                            }
                        }
                        count += c1->val - 2;
                    }
                }
                q_cells.insert (cl_info (v, count));
            }
        }
        if (q_cells.empty ()) {
            FOR_2D (v, WWW, HHH) {
                MK_C (c, v);
                if (c->flags & CELL_FLAGS_CLOSED && !(c->flags & CELL_FLAGS_MINE)) {
                    count = rand ();
                    q_cells.insert (cl_info (v, count));
                }
            }
        }
        field_open_cell (fld, q_cells.begin ()->coords);
        fld->reallocation_count.set_inf_value (fld->reallocation_count.get_inf_value () + 1);
    } else {
        fld->fail (v2i(-1,-1));
    }
}

void field::fail (v2i bad_choice) {
	SV_info s;
	get_save_status (s);
	s.statistic[gp].fails_count ++;
	set_save_status (s);

    game_over = true;
    win_time = 0.0;
	fail_choice = bad_choice;
	expl.init ((void *)this, bad_choice);
}

#undef fld

#define PI 3.14159265359
#define PI_2	(PI / 2)

double my_sin (double x) {
	if (x < 0) {
		return 0;
	} else if (x < PI_2) {
		return sin (x);
	}
	return 1;
}

int std_N[] = {3,4,4,6,8};

void explosions::init (void *fld_, v2i center) {
	field_ = fld_;
	field *fld = (field *)fld_;
	list_of_explosions.clear ();
	list_of_explosions.assign (fld->gp.mines, elem ());
	circles.setPrimitiveType (sf::Quads);
	rnd = rand () % 5;
	N = std_N[rnd];
	auto p = list_of_explosions.begin ();
	FOR_2D (v, WWW, HHH) {
		MK_C (c, v);
		if (c->flags & CELL_FLAGS_MINE) {
			p->state = 0;
			p->time = 0;
			p->pos = v;
			double l = length (!center - !v);
			if (center.x < 0) {
				l = rand1* (WWW+HHH)/4;
			}
			p->time_when_state_will_be_changed = l * 0.1 + my_sin (l/10) * rand1 * 0.5;
			p++;
		}
	}
}

void explosions::update (float dt) {
	field *fld = (field *)field_;
	circles.clear ();
	CLR blow_col = CLR (255,0,0,125);
	quad q;
	quad old;
	quad_set_color (q, blow_col);
	forstl_no_inc (p, end, list_of_explosions) {
		p->time += dt;
		switch (p->state) {
		case 0:
			if (p->time >= p->time_when_state_will_be_changed) {
				p->state = 1;
				p->time = 0.13;
			}
			if (!p->state) {
				break;
			}
		case 1:
			double out_rad = my_sin (p->time * 3) * 70 * PIX;
			double in_rad = my_sin ((p->time - 0.2) * 3) * 70 * PIX;
			if (in_rad > out_rad - 0.01) {
				p = list_of_explosions.erase (p);
				end = list_of_explosions.end ();
				continue;
			}
			if (out_rad - in_rad < 10 && (rnd == 0 || rnd == 3)) {
				blow_col.a = 125 * (out_rad - in_rad) / 10;
				quad_set_color (q, blow_col);
			}
			v2f center (v2f (fld->shift, fld->vshift) + v2f(v2i(6, 43) + p->pos*32) * PIX +
				v2f(fld->pix / 2.0f,fld->pix / 2.0f));
			FOR (i, N) {
				double ang = ((i+1)/double(N) + (rnd == 1 ? 0 : 1)/(2.0*N))* PI* 2;
				v2f k (sin (ang), cos (ang));
				q[2].position = center + k * float(out_rad);
				q[3].position = center + k * float(in_rad);
				if (i) {
					q[0] = old[3];
					q[1] = old[2];
				} else {
					ang = (i/double(N) + (rnd == 1 ? 0 : 1)/(2.0*N))* PI* 2;
					k = v2f(sin (ang), cos (ang));
					q[1].position = center + k * float(out_rad);
					q[0].position = center + k * float(in_rad);
				}
				quad_v_a (q, &circles);
				FOR (z, 4) {
					old[z] = q[z];
				}
			}
			if (out_rad - in_rad < 10 && (rnd == 0 || rnd == 3)) {
				blow_col.a = 125;
				quad_set_color (q, blow_col);
			}
			break;
		}
		++p;
	}
}

void explosions::draw (sf::RenderWindow *wind) {
	wind->draw (circles);
}