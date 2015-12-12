#include "animation.h"
#include "input.h"
#include <list>

#define MK_C(c, point)	float *c = &((*an)[point]);

cell_ar <float> *an = NULL;
cell_ar <int> *ian = NULL;
#define AN	(*an)
#define IAN (*ian)

int smpl[11][9] =
{{0,0,0,1,1,1,0,0,1},
 {1,1,0,0,1,0,0,1,0},
 {0,1,0,0,1,0,1,1,0},
 {1,1,1,0,1,0,0,0,0},

 {0,1,0,0,1,1,0,1,0},
 {1,1,0,1,1,0,0,0,0},
 {1,1,0,0,1,0,0,1,0},
 {1,1,0,0,1,0,0,1,0},

 {1,1,0,0,1,1,0,0,0},
 {0,1,0,0,1,1,0,0,1},
 {1,0,0,0,0,0,0,0,0}
};

struct shape {
	int *ar;
	int clock;
	int x_coord;
	shape (int x_c, int num_of_shape) : clock (0), x_coord (x_c) {ar = smpl[num_of_shape];}
};
int sw = 0;

std::list <shape> sps;

int intro_anim3 (field *fld, float dt) {
	if(input.mbutton[MOUSE_LEFT].just_pressed) {
		delete an;
		an = NULL;
		return true;
	}
	if (!an) {
		an = new cell_ar <float> (HHH, WWW);
		FOR_2D (v, WWW, HHH) {
			AN[v] = 0;
		}
	}
	dt *= WWW / 30.0;
	fld->time += dt;
	FOR_2D (v, WWW, HHH) {
		if (v.y & 1) {
			if (length (v2f (0, HHH) - v2f(v.x, v.y*2)) < sqrt(fld->time)*30) {
				if (AN[v] < 0.001) {
					AN[v] = 0.001;
				}
			}
			if (AN[v]) {
				AN[v] += dt*4;//0.4/length (v2f (0, HHH) - v2f(v.x, v.y*2)) * fld->time;
			}
		} else {
			if (length (v2f (WWW, HHH) - v2f(v.x, v.y * 2)) < sqrt(fld->time)*30) {
				if (AN[v] < 0.001) {
					AN[v] = 0.001;
				}
			}
			if (AN[v]) {
				AN[v] += dt*4;//0.4 / length (v2f (WWW, HHH) - v2f(v.x, v.y * 2)) * fld->time;
			}
		}
	}

	fld->cells.clear ();
	quad q;
	quad_set_tex_rect (q, SFRECTcv (int, 0,0,fld->pix > 48 ? v2f(96,96) : v2f (48, 48)));
	FOR_2D (v, WWW, HHH) {
		quad_set_pos (q, v2f(fld->shift, fld->vshift) + v2f(v2i(6, 43) + v*32) * PIX, v2f(fld->pix,fld->pix),false);
		MK_C (c, v);
		*c = Min (1, *c);
		*c = Max (0, *c);
		quad_set_color (q, (my_clr_s[color_theme].unknown * (*c)) + (my_clr_s[color_theme].safe * (1 - *c)));
		quad_v_a (q, &fld->cells);
	}
	quad_set_tex_rect (q, sf::IntRect (4, 4, 40, 40));
	quad_set_pos (q, v2f(fld->shift ,0), v2f(MY_WIND_WIDTH, 37) * PIX, 0);
	quad_set_color (q, my_clr_s[color_theme].unknown);
	quad_v_a (q, &fld->cells);
	FOR_2D (v, WWW, HHH) {
		if (AN[v] < 1) {
			return false;
		}
	}
	fld->time = 0;
	delete an;
	an = NULL;
	return true;
}

int intro_anim4 (field *fld, float dt) {
	if(input.mbutton[MOUSE_LEFT].just_pressed) {
		delete an;
		an = NULL;
		return true;
	}
	if (!an) {
		an = new cell_ar <float> (HHH, WWW);
		FOR_2D (v, WWW, HHH) {
			AN[v] = 0;
		}
	}
	dt *= WWW / 30.0;
	fld->time += dt;
	FOR_2D (v, WWW, HHH) {
		if (v.x & 1) {
			if (length (v2f (0, HHH) - v2f(v.x, v.y*2)) < sqrt(fld->time)*30) {
				if (AN[v] < 0.001) {
					AN[v] = 0.001;
				}
			}
			if (AN[v]) {
				AN[v] += dt*4;//0.4/length (v2f (0, HHH) - v2f(v.x, v.y*2)) * fld->time;
			}
		} else {
			if (length (v2f (WWW, HHH) - v2f(v.x, v.y * 2)) < sqrt(fld->time)*30) {
				if (AN[v] < 0.001) {
					AN[v] = 0.001;
				}
			}
			if (AN[v]) {
				AN[v] += dt*4;//0.4 / length (v2f (WWW, HHH) - v2f(v.x, v.y * 2)) * fld->time;
			}
		}
	}

	fld->cells.clear ();
	quad q;
	quad_set_tex_rect (q, SFRECTcv (int, 0,0,fld->pix > 48 ? v2f(96,96) : v2f (48, 48)));
	FOR_2D (v, WWW, HHH) {
		quad_set_pos (q, v2f(fld->shift, fld->vshift) + v2f(v2i(6, 43) + v*32) * PIX, v2f(fld->pix,fld->pix),false);
		MK_C (c, v);
		*c = Min (1, *c);
		*c = Max (0, *c);
		quad_set_color (q, (my_clr_s[color_theme].unknown * (*c)) + (my_clr_s[color_theme].safe * (1 - *c)));
		quad_v_a (q, &fld->cells);
	}
	quad_set_tex_rect (q, sf::IntRect (4, 4, 40, 40));
	quad_set_pos (q, v2f(fld->shift ,0), v2f(MY_WIND_WIDTH, 37) * PIX, 0);
	quad_set_color (q, my_clr_s[color_theme].unknown);
	quad_v_a (q, &fld->cells);
	FOR_2D (v, WWW, HHH) {
		if (AN[v] < 1) {
			return false;
		}
	}
	fld->time = 0;
	delete an;
	an = NULL;
	return true;
}

int intro_anim5 (field *fld, float dt) {
	if(input.mbutton[MOUSE_LEFT].just_pressed) {
		delete an;
		an = NULL;
		return true;
	}
	if (!an) {
		an = new cell_ar <float> (HHH, WWW);
		FOR_2D (v, WWW, HHH) {
			AN[v] = 0;
		}
	}
	dt *= WWW / 30.0;
	fld->time += dt;
	FOR_2D (v, WWW, HHH) {
		if (v.x & 1 && v.y & 1) {
			if (length (v2f (0, 0) - v2f(v.x, v.y*2)) < sqrt(fld->time)*30) {
				if (AN[v] < 0.001) {
					AN[v] = 0.001;
				}
			}
			if (AN[v]) {
				AN[v] += dt*4;//0.4/length (v2f (0, HHH) - v2f(v.x, v.y*2)) * fld->time;
			}
		} else if (v.x & 1 && !(v.y & 1)) {
			if (length (v2f (0, 2*HHH) - v2f(v.x, v.y * 2)) < sqrt(fld->time)*30) {
				if (AN[v] < 0.001) {
					AN[v] = 0.001;
				}
			}
			if (AN[v]) {
				AN[v] += dt*4;//0.4 / length (v2f (WWW, HHH) - v2f(v.x, v.y * 2)) * fld->time;
			}
		} else if (!(v.x & 1) && (v.y & 1)) {
			if (length (v2f (WWW, 0) - v2f(v.x, v.y * 2)) < sqrt(fld->time)*30) {
				if (AN[v] < 0.001) {
					AN[v] = 0.001;
				}
			}
			if (AN[v]) {
				AN[v] += dt*4;//0.4 / length (v2f (WWW, HHH) - v2f(v.x, v.y * 2)) * fld->time;
			}
		} else if (!(v.x & 1) && !(v.y & 1)) {
			if (length (v2f (WWW, 2*HHH) - v2f(v.x, v.y * 2)) < sqrt(fld->time)*(30)) {
				if (AN[v] < 0.001) {
					AN[v] = 0.001;
				}
			}
			if (AN[v]) {
				AN[v] += dt*4;//0.4 / length (v2f (WWW, HHH) - v2f(v.x, v.y * 2)) * fld->time;
			}
		}
	}

	fld->cells.clear ();
	quad q;
	quad_set_tex_rect (q, SFRECTcv (int, 0,0,fld->pix > 48 ? v2f(96,96) : v2f (48, 48)));
	FOR_2D (v, WWW, HHH) {
		quad_set_pos (q, v2f(fld->shift, fld->vshift) + v2f(v2i(6, 43) + v*32) * PIX, v2f(fld->pix,fld->pix),false);
		MK_C (c, v);
		*c = Min (1, *c);
		*c = Max (0, *c);
		quad_set_color (q, (my_clr_s[color_theme].unknown * (*c)) + (my_clr_s[color_theme].safe * (1 - *c)));
		quad_v_a (q, &fld->cells);
	}
	quad_set_tex_rect (q, sf::IntRect (4, 4, 40, 40));
	quad_set_pos (q, v2f(fld->shift ,0), v2f(MY_WIND_WIDTH, 37) * PIX, 0);
	quad_set_color (q, my_clr_s[color_theme].unknown);
	quad_v_a (q, &fld->cells);
	FOR_2D (v, WWW, HHH) {
		if (AN[v] < 1) {
			return false;
		}
	}
	fld->time = 0;
	delete an;
	an = NULL;
	return true;
}


int intro_anim1 (field *fld, float dt) {
	if(input.mbutton[MOUSE_LEFT].just_pressed) {
		delete an;
		delete ian;
		an = NULL;
		ian = NULL;
		sps.clear ();
		return true;
	}
	if (!an) {
		an = new cell_ar <float> (HHH, WWW);
		ian = new cell_ar <int> (HHH, WWW);
		FOR (i, (HHH+2)*(WWW+2)) {
			an->f[i] = 1;
		}
		FOR_2D (v, WWW, HHH) {
			AN[v] = 0;
		}
	}
	if (sw) {
		sw = 0;
	} else {
		int n = rand1 * exp (exp(fld->time*0.7) - 1);
		For (n) {
			sps.push_back (shape(rand () % (WWW - 1), rand () % 10));
		}
		FOR (i, WWW) {
			AN[v2i(i, -1)] = 0;
		}
		forlist_no_inc (p, end, shape, sps) {
			FOR (i, 3) {
				if (p->ar[i*3 + p->clock]) {
					AN[v2i (i + p->x_coord, -1)] = 1;
				}
			}
			if (++p->clock == 3) {
				p = sps.erase (p);
			} else {
				++p;
			}
		}
		fld->time += dt;

		FOR_2D (v, WWW, HHH) {
			IAN[v] = 0;
		}
		FOR (i, WWW) {
			v2i v (i, -1);
			if (AN[v]) {
				int flag = 1;
				for (int j = v.y+1; j <= HHH; ++j) {
					if (!AN[v2i(v.x, j)]) {
						flag = 0;
					}
				}
				if (flag) {
					IAN[v] = 1;
				} else {
					IAN[v + v2i(0,1)] = 1;
				}
			}
		}
		FOR_2D (v, WWW, HHH) {
			if (AN[v]) {
				int flag = 1;
				for (int j = v.y+1; j <= HHH; ++j) {
					if (!AN[v2i(v.x, j)]) {
						flag = 0;
					}
				}
				if (flag) {
					IAN[v] = 1;
				} else {
					IAN[v + v2i(0,1)] = 1;
				}
			}
		}

		FOR_2D (v, WWW, HHH) {
			(*an)[v] = (*ian)[v];
		}
		sw = 0;
	}


	fld->cells.clear ();
	quad q;
	quad_set_tex_rect (q, SFRECTcv (int, 0,0,fld->pix > 48 ? v2f(96,96) : v2f (48, 48)));
	FOR_2D (v, WWW, HHH) {
		quad_set_pos (q, v2f(fld->shift, fld->vshift) + v2f(v2i(6, 43) + v*32) * PIX, v2f(fld->pix,fld->pix),false);
		MK_C (c, v);
		*c = Min (1, *c);
		*c = Max (0, *c);
		quad_set_color (q, (my_clr_s[color_theme].unknown * (*c)) + (my_clr_s[color_theme].safe * (1 - *c)));
		quad_v_a (q, &fld->cells);
	}
	quad_set_tex_rect (q, sf::IntRect (4, 4, 40, 40));
	quad_set_pos (q, v2f(fld->shift ,0), v2f(MY_WIND_WIDTH, 37) * PIX, 0);
	quad_set_color (q, my_clr_s[color_theme].unknown);
	quad_v_a (q, &fld->cells);
	FOR_2D (v, WWW, HHH) {
		if (AN[v] == 0) {
			return false;
		}
	}
	fld->time = 0;
	delete an;
	delete ian;
	an = NULL;
	ian = NULL;
	sps.clear ();
	return true;
}

int intro_anim2 (field *fld, float dt) {
	if(input.mbutton[MOUSE_LEFT].just_pressed) {
		delete an;
		delete ian;
		an = NULL;
		ian = NULL;
		sps.clear ();
		return true;
	}
	if (!an) {
		an = new cell_ar <float> (HHH, WWW);
		ian = new cell_ar <int> (HHH, WWW);
		FOR (i, (HHH+2)*(WWW+2)) {
			an->f[i] = 1;
		}
		FOR_2D (v, WWW, HHH) {
			AN[v] = 0;
		}
	}
	if (sw) {
		sw = 0;
	} else {
		int n = rand1 * exp (exp(fld->time) - 1);
		For (n)
			sps.push_back (shape (rand () % (WWW), 10));
		FOR (i, WWW) {
			AN[v2i(i, -1)] = 0;
		}
		forlist_no_inc (p, end, shape, sps) {
			FOR (i, 3) {
				if (p->ar[i*3 + p->clock]) {
					AN[v2i (i + p->x_coord, -1)] = 1;
				}
			}
			if (++p->clock == 3) {
				p = sps.erase (p);
			} else {
				++p;
			}
		}
		fld->time += dt;

		FOR_2D (v, WWW, HHH) {
			IAN[v] = 0;
		}
		FOR (i, WWW) {
			v2i v (i, -1);
			if (AN[v]) {
				int flag = 1;
				for (int j = v.y+1; j <= HHH; ++j) {
					if (!AN[v2i(v.x, j)]) {
						flag = 0;
					}
				}
				if (flag) {
					IAN[v] = 1;
				} else {
					IAN[v + v2i(0,1)] = 1;
				}
			}
		}
		FOR_2D (v, WWW, HHH) {
			if (AN[v]) {
				int flag = 1;
				for (int j = v.y+1; j <= HHH; ++j) {
					if (!AN[v2i(v.x, j)]) {
						flag = 0;
					}
				}
				if (flag) {
					IAN[v] = 1;
				} else {
					IAN[v + v2i(0,1)] = 1;
				}
			}
		}

		FOR_2D (v, WWW, HHH) {
			(*an)[v] = (*ian)[v];
		}
		sw = 0;
	}


	fld->cells.clear ();
	quad q;
	quad_set_tex_rect (q, SFRECTcv (int, 0,0,fld->pix > 48 ? v2f(96,96) : v2f (48, 48)));
	FOR_2D (v, WWW, HHH) {
		quad_set_pos (q, v2f(fld->shift, fld->vshift) + v2f(v2i(6, 43) + v*32) * PIX, v2f(fld->pix,fld->pix),false);
		MK_C (c, v);
		*c = Min (1, *c);
		*c = Max (0, *c);
		quad_set_color (q, (my_clr_s[color_theme].unknown * (*c)) + (my_clr_s[color_theme].safe * (1 - *c)));
		quad_v_a (q, &fld->cells);
	}
	quad_set_tex_rect (q, sf::IntRect (4, 4, 40, 40));
	quad_set_pos (q, v2f(fld->shift ,0), v2f(MY_WIND_WIDTH, 37) * PIX, 0);
	quad_set_color (q, my_clr_s[color_theme].unknown);
	quad_v_a (q, &fld->cells);
	FOR_2D (v, WWW, HHH) {
		if (AN[v] == 0) {
			return false;
		}
	}
	fld->time = 0;
	delete an;
	delete ian;
	an = NULL;
	ian = NULL;
	sps.clear ();
	return true;
}

int (*intro_anim[5]) (field *fld, float dt) = {
	intro_anim1,
	intro_anim2,
	intro_anim3,
	intro_anim4,
	intro_anim5
};