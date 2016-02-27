#include "field_utility.h"
#include "slau.h"

v2i dr[8] = {
	v2i( 0, 1),
	v2i( 0,-1),
	v2i( 1, 0),
	v2i(-1, 0),
	v2i( 1,-1),
	v2i( 1, 1),
	v2i(-1,-1),
	v2i(-1, 1)
};

struct hlp {
	std::list <v2i> het;
	std::list <v2i> hom;
	bool n_a;
};

#define FOR_NEIGHBORS(r, Center_)	v2i r = Center_ + dr[0]; for (int z = 0; z < 8; ++z, r = Center_ + dr[z])

void field_make (field *fld, std::list <v2i> *free_cells, int mines) {
	For (mines) {
		int nxt = rand_n (free_cells->size ());
		v2i mine_pos;
		forlist (p, end, v2i, *free_cells) {
			if (!(nxt--)) {
				mine_pos = *p;
				free_cells->erase (p);
				break;
			}
		}
		fld->a[mine_pos].flags |= CELL_FLAGS_MINE;
	}
	int sum;
	FOR_2D (v, fld->gp.width, fld->gp.height) {
		fld->a[v].flags |= CELL_FLAGS_CLOSED;
		if (!(fld->a[v].flags & CELL_FLAGS_MINE)) {
			sum = 0;
			FOR_NEIGHBORS (r, v) {
				if (fld->a[r].flags & CELL_FLAGS_MINE) {
					++sum;
				}
			}
			fld->a[v].val = sum;
		} else {
			fld->a[v].val = 0;
		}
	}
}

#include "cell.h"

struct q_point {
    v2i coord;
    int flags;
    q_point (v2i c) : coord (c), flags (0) {}
};

bool field_assumption_ok (field *fld, v2i ch) {
    int count_closed;
    int count_p_mine;
    int count_p_safe;
	FOR_NEIGHBORS (v, ch) {
        MK_C (c, v);
        if (!(c->flags & CELL_FLAGS_CLOSED)) {
            if (c->val) {
                count_closed = 0;
                count_p_mine = 0;
                count_p_safe = 0;
                FOR_NEIGHBORS (r, v) {
                    MK_C (c1, r);
					if (c1->flags & CELL_FLAGS_CLOSED && POINT_IN_RECT (r, 0,0, WWW, HHH)) {
                        ++count_closed;
                        if (c1->flags & CELL_P_MINE) {
                            ++count_p_mine;
                        } else if (c1->flags & CELL_P_SAFE) {
                            ++count_p_safe;
                        }
                    }
                }
                if (count_p_mine > c->val || count_closed - count_p_safe < c->val) {
                    return false;
                }
            }
        }
    }
    return true;
}

/*
bool rec_solve (field *fld, std::list <q_point>::iterator p, std::list<q_point> *l, bool one = false, int free_count = 0, int const_mines = 0) {
    std::list <q_point>::iterator next = p;
    ++next;
    fld->a[p->coord].flags |= CELL_P_MINE;
	if (field_assumption_ok (fld, p->coord)) {
        if (next == l->end ()) {
			if (one) {
				int mine_c = 0;
				forlist (p, end, q_point, *l) {
					if (fld->a[p->coord].flags & CELL_P_MINE) {
						++mine_c;
					}
				}
				if (const_mines + mine_c + free_count >= fld->gp.mines) {
					forlist (p, end, q_point, *l) {
						if (fld->a[p->coord].flags & CELL_P_SAFE) {
							p->flags |= CELL_P_SAFE;
						} else {
							p->flags |= CELL_P_MINE;
						}
					}
					return true;
				}
			} else {
				forlist (p, end, q_point, *l) {
					if (fld->a[p->coord].flags & CELL_P_SAFE) {
						p->flags |= CELL_P_SAFE;
					} else {
						p->flags |= CELL_P_MINE;
					}
				}
			}
        } else {
			if (rec_solve (fld, next, l, one, free_count, const_mines)) {
				fld->a[p->coord].flags &= ~CELL_P_MINE;
				return true;
			}
        }
    }
    fld->a[p->coord].flags &= ~CELL_P_MINE;
    fld->a[p->coord].flags |= CELL_P_SAFE;
	if (field_assumption_ok (fld, p->coord)) {
        if (next == l->end ()) {
            if (one) {
				int mine_c = 0;
				forlist (p, end, q_point, *l) {
					if (fld->a[p->coord].flags & CELL_P_MINE) {
						++mine_c;
					}
				}
				if (const_mines + mine_c + free_count >= fld->gp.mines) {
					forlist (p, end, q_point, *l) {
						if (fld->a[p->coord].flags & CELL_P_SAFE) {
							p->flags |= CELL_P_SAFE;
						} else {
							p->flags |= CELL_P_MINE;
						}
					}
					return true;
				}
			} else {
				forlist (p, end, q_point, *l) {
					if (fld->a[p->coord].flags & CELL_P_SAFE) {
						p->flags |= CELL_P_SAFE;
					} else {
						p->flags |= CELL_P_MINE;
					}
				}
			}
        } else {
            if (rec_solve (fld, next, l, one, free_count, const_mines)) {
				fld->a[p->coord].flags &= ~CELL_P_SAFE;
				return true;
			}
        }
    }
    fld->a[p->coord].flags &= ~CELL_P_SAFE;
	return false;
}
*/

#include <Windows.h>

void field_solve (field *fld) {
	lll:
	std::vector <v2i> q;
	std::set <v2i> new_cells;
	cell_ar <int> inv (HHH, WWW);
	inv.init (-1);
    FOR_2D (v, WWW, HHH) {
        MK_C (c, v);
		TRN_OFF (c->flags, CELL_Q);
		if (!(c->flags & CELL_P_MINE || c->flags & CELL_P_SAFE || c->flags & CELL_Q) && (c->flags & CELL_FLAGS_CLOSED)) {
            FOR_NEIGHBORS (r, v) {
                if (POINT_IN_RECT (r, 0, 0, WWW, HHH)) {
                    MK_C (cr, r);
                    if (! (cr->flags & CELL_FLAGS_CLOSED)) {
                        c->flags |= CELL_Q;
                        break;
                    }
                }
            }
        }
        if (c->flags & CELL_Q) {
			q.push_back (v);
			if (fld->was_q.find (v) == fld->was_q.end ()) {
				new_cells.insert (v);
			}
			inv[v] = q.size () - 1;
        }
    }
	if (!q.empty ()) {
		std::list <row_for_slau_info> tmp_m_slau;
		row_for_slau_info tmp_new;
		std::set <int> new_elem;

		FOR_2D (v, WWW, HHH) {
			MK_C (c, v);
			if (!(c->flags & CELL_FLAGS_CLOSED)) {
				int r_v = c->val;
				new_elem.clear ();
				FOR_NEIGHBORS (r, v) {
					if (POINT_IN_RECT (r, 0, 0, WWW, HHH)) {
						MK_C (cr, r);
						if (cr->flags & CELL_FLAGS_CLOSED) {
							if (cr->flags & CELL_P_MINE) {
								--r_v;
							}
							if (cr->flags & CELL_Q) {
								new_elem.insert (inv[r]);
							}
						}
					}
				}
				if (!new_elem.empty ()) {
					tmp_new.non_zero_elements = new_elem;
					tmp_new.right_b = r_v;
					tmp_m_slau.push_back (tmp_new);
				}
			}
		}
		bool cont = false;
		{
			std::list <row_for_slau_info> rl;
			std::set <int> in;
			forset (r, end, v2i, new_cells) {
				in.insert (inv[*r]);
			}
			forlist (p, end, row_for_slau_info, tmp_m_slau) {
				bool ins = false;
				forset (r, end, v2i, new_cells) {
					if (p->non_zero_elements.find (inv[*r]) != p->non_zero_elements.end ()) {
						ins = true;
						break;
					}
				}
				if (ins) {
					rl.push_back (*p);
					in.insert (p->non_zero_elements.begin (), p->non_zero_elements.end ());
				}
			}
			slau_1 sl_1 (rl.size (), in.size ());
			std::vector <int> num;
			num.assign (q.size (), 0);
			int i = 0;
			forset(p, end, int, in) {
				sl_1.link.push_back (q[*p]);
				num[*p] = i++;
			}
			i = 0;
			forlist (p, end, row_for_slau_info, rl) {
				forset (r, end, int, p->non_zero_elements) {
					sl_1.A_b[i][num[*r]] = 1;
				}
				sl_1.A_b[i][sl_1.w_A_with_b - 1] = p->right_b;
				++i;
			}
		
			slau_2 sl_2 = sl_1.solve ();
			sl_2.solve ();
		
			v2i tmp_p;
			FOR (i, sl_2.h) {
				tmp_p = sl_2.link_h[i];
				//fld->was_q.insert (tmp_p);
				if (GET_F_STATE (sl_2.inf_h[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ONE)) {
					if (!GET_F_STATE (sl_2.inf_h[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ZERO)) {
						fld->a[tmp_p].flags &= ~CELL_Q;
						fld->a[tmp_p].flags |= CELL_P_MINE;
						if (new_cells.find (tmp_p) == new_cells.end ()) {
							cont = true;
						}
					}
				} else {
					fld->a[tmp_p].flags &= ~CELL_Q;
					fld->a[tmp_p].flags |= CELL_P_SAFE;
					if (new_cells.find (tmp_p) == new_cells.end ()) {
						cont = true;
					}
				}
			}
		
			FOR (i, sl_2.w_A_with_b - 1) {
				tmp_p = sl_2.link_w[i];
				//fld->was_q.insert (tmp_p);
				if (GET_F_STATE (sl_2.inf_w[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ONE)) {
					if (!GET_F_STATE (sl_2.inf_w[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ZERO)) {
						
						fld->a[tmp_p].flags &= ~CELL_Q;
						fld->a[tmp_p].flags |= CELL_P_MINE;
						if (new_cells.find (tmp_p) == new_cells.end ()) {
							cont = true;
						}
					}
				} else {
					fld->a[tmp_p].flags &= ~CELL_Q;
					fld->a[tmp_p].flags |= CELL_P_SAFE;
					if (new_cells.find (tmp_p) == new_cells.end ()) {
						cont = true;
					}
				}
			}
		}
		while (cont && !tmp_m_slau.empty ()) {
			std::list <row_for_slau_info> rl; // содержит информацию о всех строках
			rl.push_back (*tmp_m_slau.begin ());
			tmp_m_slau.erase (tmp_m_slau.begin ());
			std::set<int> in; // номера клеток в списке q, вошедшие в СЛАУ
			in = rl.begin ()->non_zero_elements;
			bool ch;
			do {
				ch = false;
				forlist_no_inc (p, end, row_for_slau_info, tmp_m_slau) {
					bool ins = false;
					forset (r, end, int, in) {
						if (p->non_zero_elements.find (*r) != p->non_zero_elements.end ()) {
							ins = true;
							ch = true;
							break;
						}
					}
					if (ins) {
						rl.push_back (*p);
						in.insert (p->non_zero_elements.begin (), p->non_zero_elements.end ());
						p = tmp_m_slau.erase (p);
					} else {
						++p;
					}
				}
			} while (ch);
			slau_1 sl_1 (rl.size (), in.size ());
			std::vector <int> num;
			num.assign (q.size (), 0);
			int i = 0;
			bool nw = false;
			forset(p, end, int, in) {
				if (fld->was_q.find (q[*p]) == fld->was_q.end ()) {
					nw = true;
				}
				sl_1.link.push_back (q[*p]);
				num[*p] = i++;
			}
			if (!nw) {
				continue;
			}
			i = 0;
			forlist (p, end, row_for_slau_info, rl) {
				forset (r, end, int, p->non_zero_elements) {
					sl_1.A_b[i][num[*r]] = 1;
				}
				sl_1.A_b[i][sl_1.w_A_with_b - 1] = p->right_b;
				++i;
			}
		
			slau_2 sl_2 = sl_1.solve ();
			sl_2.solve ();
		
			v2i tmp_p;
			FOR (i, sl_2.h) {
				tmp_p = sl_2.link_h[i];
				fld->was_q.insert (tmp_p);
				if (GET_F_STATE (sl_2.inf_h[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ONE)) {
					if (!GET_F_STATE (sl_2.inf_h[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ZERO)) {
						fld->a[tmp_p].flags &= ~CELL_Q;
						fld->a[tmp_p].flags |= CELL_P_MINE;
					}
				} else {
					fld->a[tmp_p].flags &= ~CELL_Q;
					fld->a[tmp_p].flags |= CELL_P_SAFE;
				}
			}
		
			FOR (i, sl_2.w_A_with_b - 1) {
				tmp_p = sl_2.link_w[i];
				fld->was_q.insert (tmp_p);
				if (GET_F_STATE (sl_2.inf_w[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ONE)) {
					if (!GET_F_STATE (sl_2.inf_w[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ZERO)) {
						
						fld->a[tmp_p].flags &= ~CELL_Q;
						fld->a[tmp_p].flags |= CELL_P_MINE;
					}
				} else {
					fld->a[tmp_p].flags &= ~CELL_Q;
					fld->a[tmp_p].flags |= CELL_P_SAFE;
				}
			}
		}
	}

	fld->no_moves = true;
	FOR_2D (v, WWW, HHH) {
		MK_C (c, v);
		if ((c->flags & CELL_FLAGS_CLOSED) && (c->flags & CELL_P_SAFE)) {
			fld->no_moves = false;
		}
	}
}

void field_open_cell (field *fld, v2i choose) {
	if (!POINT_IN_RECT (choose, 0, 0, WWW, HHH) ) {
		return;
	}
	if (!(fld->a[choose].flags & CELL_FLAGS_CLOSED) || (fld->a[choose].flags & CELL_FLAGS_MARK)) {
		return;
	}
	if (fld->a[choose].flags & CELL_FLAGS_MINE) {
		if (fld->no_moves && (fld->a[choose].flags & CELL_Q)) {
			fld->a[choose].flags &= ~CELL_Q;
			fld->a[choose].flags |= CELL_P_SAFE;

			int free_count = 0;
			int const_mines = 0;
			FOR_2D (v, WWW, HHH) {
				MK_C (c, v);
				if ((c->flags & CELL_FLAGS_CLOSED) && (c->flags & CELL_P_MINE)) {
					++const_mines;
				} else if ((c->flags & CELL_FLAGS_CLOSED) && !(c->flags & CELL_Q)) {
					++free_count;
				}
			}

			std::list <v2i> q;
			FOR_2D (v, WWW, HHH) {
				MK_C (c, v);
				if (!(c->flags & CELL_FLAGS_CLOSED)) {
					c->flags &= ~CELL_Q;
				}
				if (!(c->flags & CELL_P_MINE || c->flags & CELL_P_SAFE || c->flags & CELL_Q) && (c->flags & CELL_FLAGS_CLOSED)) {
					FOR_NEIGHBORS (r, v) {
						if (POINT_IN_RECT (r, 0, 0, WWW, HHH)) {
							MK_C (cr, r);
							if (! (cr->flags & CELL_FLAGS_CLOSED)) {
								c->flags |= CELL_Q;
								break;
							}
						}
					}
				}
				if ((c->flags & CELL_Q) && (v != choose)) {
					q.push_back (v);
				}
			}
			int new_mines = 0;
			if (!q.empty ()) {
				std::list <q_point> q_p;
				cell_ar <cell> a (HHH, WWW);
				cell_ar <hlp> h (HHH, WWW);
				for (int i = -1; i <= fld->gp.height; ++i) {
					for (int j = -1; j <= fld->gp.width; ++j) {
						a[i][j].flags = 100;
						a[i][j].val = 0;
						h[i][j].n_a = false;
					}
				}
				FOR_2D (v, WWW, HHH) {
					MK_C (c, v);
					if (!(c->flags & CELL_FLAGS_CLOSED)) {
						a[v].val = -1;
					}
				}
				FOR_2D (v, WWW, HHH) {
					FOR_NEIGHBORS (r, v) {
						a[v].flags += a[r].val;
					}
				}
				FOR_2D (v, WWW, HHH) {
					if (fld->a[v].flags & CELL_FLAGS_CLOSED) {
						FOR_NEIGHBORS (r, v) {
							if (!(fld->a[r].flags & CELL_FLAGS_CLOSED)) {
								h[v].het.push_back (r);
								FOR_NEIGHBORS (u, r) {
									if ((fld->a[u].flags & CELL_FLAGS_CLOSED) && (fld->a[u].flags & CELL_Q)) {
										bool add = true;
										forlist (p, end, v2i, h[v].hom) {
											if (*p == u) {
												add = false;
												break;
											}
										}
										if (add) {
											h[v].hom.push_back (u);
										}
									}
								}
							}
						}
					}
				}
				while (!q.empty ()) {
					v2i last (-1,-1);
					bool first = true;
					while (!q.empty ()) {
						v2i mp;
						int max_v = 0;
						std::list <v2i>::iterator del;
						int bon;
						bool found_one = false;
						forlist (it, end, v2i, q) {
							if (first || h[*it].n_a) {
								if (abs(it->x - last.x) + abs (it->y - last.y) == 1) {
									bon = 200;
								} else {
									bon = 0;
								}
								if ((a[*it].flags) + bon > max_v) {
									del = it;
									mp = *it;
									max_v = a[*it].flags;
									found_one = true;
								}
							}
						}
						first = false;
						if (!found_one) {
							break;
						}
						q.erase (del);
						q_p.push_back (mp);
						forlist (p, end, v2i, h[mp].hom) {
							a[*p].flags += (10-h[mp].hom.size ()) * (10 - h[*p].hom.size());
							h[*p].n_a = true;
						}
						last = mp;
					}
					
					/*
					sf::Event event;
					while (window.pollEvent(event))
					{
					}
					int count = 0;
					forlist (p, end, q_point, q_p) {
						fld->sec.setPosition ( v2f(v2i(7, 44) + p->coord*32));
						fld->sec.setString (std::to_string (count));
						window.draw (fld->sec);
						++count;
					}
					window.display ();
					while (!kb::isKeyPressed (kb::G)) {
						Sleep (10);
					}
					*/

					fld->a[choose].flags |= CELL_P_SAFE;
					//rec_solve (fld, q_p.begin (), &q_p, true, free_count, const_mines + new_mines);
					forlist (p, end, q_point, q_p) {
						if (!((p->flags & CELL_P_MINE) || (p->flags & CELL_P_SAFE))) {
							fld->game_over = true;
							return;
						} else {
							MK_C (c, p->coord);
							c->flags &= ~CELL_P_SAFE;
							c->flags &= ~CELL_P_MINE;
							if (p->flags & CELL_P_MINE) {
								c->flags |= CELL_Q;
								c->flags |= CELL_FLAGS_MINE;
								++new_mines;
							} else {
								c->flags |= CELL_Q;
								c->flags &= ~CELL_FLAGS_MINE;
							}
						}
					}
				}
			}
			std::list <v2i> fr_c;
				FOR_2D (v, WWW, HHH) {
				MK_C (c, v);
				if ((c->flags & CELL_FLAGS_CLOSED) && !((c->flags & CELL_P_MINE) || (c->flags & CELL_P_SAFE) || (c->flags & CELL_Q)) && v != choose) {
					fr_c.push_back (v);
					c->flags &= ~CELL_FLAGS_MINE;
				}
			}
			fld->a[choose].flags = 0;
			const_mines = 0;
			FOR_2D (v, WWW, HHH) {
				MK_C (c, v);
				if ((c->flags & CELL_P_MINE) || ((c->flags & CELL_Q) && (c->flags & CELL_FLAGS_MINE))) {
					++const_mines;
				}
			}
			field_make (fld, &fr_c, fld->gp.mines - const_mines);
			fld->no_moves = true;
			FOR_2D (v, WWW, HHH) {
				MK_C (c, v);
				if ((c->flags & CELL_FLAGS_CLOSED) && (c->flags & CELL_P_SAFE)) {
					fld->no_moves = false;
				}
			}
			sf::Clock timer;
			timer.restart ();
			field_solve (fld);
			fld->mkr +=	(timer.getElapsedTime ()).asMicroseconds ();
		} else {
			fld->game_over = true;
			return;
		}
	}
	if (!(fld->a[choose].flags & CELL_FLAGS_CLOSED)) {
		return;
	}
	cell_ar <cell> ca (fld->gp.height, fld->gp.width);
	for (v2i v_ = v2i(-1,-1); v_.x <= WWW; ++v_.x) {
		for (v_.y = -1; v_.y <= HHH; ++v_.y) {
			ca[v_].val = 0;
			ca[v_].flags = 0;
		}
	}
	std::list <v2i> dots[2];
	int sw = 0;
	if (!fld->a[choose].val) {
		dots[sw].push_back (choose);
	}
	ca[choose].val = 1;
	ca[choose].flags = 1;
	
	while (!dots[sw].empty ()) {
		dots[!sw].clear ();
		forlist (p, end, v2i, dots[sw]) {
			FOR_NEIGHBORS (r, *p) {
				MK_C (c, r);
				if ((c->flags & CELL_FLAGS_MINE) || (c->flags & CELL_FLAGS_MARK)) {
					continue;
				} else {
					if (c->flags & CELL_FLAGS_CLOSED) {
						ca[r].val = 1;
					}
					if (POINT_IN_RECT (r, 0, 0, WWW, HHH) && !c->val && !ca[r].flags) {
						dots[!sw].push_back (r);
						ca[r].flags = 1;
					}
				}
			}
		}
		sw = !sw;
	}
	
	FOR_2D(v, WWW, HHH) {
		if (ca[v].val) {
			MK_C (c, v);
			c->flags &= ~CELL_FLAGS_CLOSED;
			if ( v != choose) {
				force_cell fc0 (v, my_clr_s[color_theme].unknown, CLR_BLIND (my_clr_s[color_theme].safe), 0.1, 0.03 * length (v2f(v - choose)));
				force_cell fc1 (v, my_clr_s[color_theme].unknown, my_clr_s[color_theme].unknown, 0.03 * length (v2f(v - choose)));
				fld->fc_l.push_back (fc0);
				fld->fc_l.push_back (fc1);
			}
		}
	}
	sf::Clock timer;
    timer.restart ();
	field_solve (fld);
	fld->mkr +=	(timer.getElapsedTime ()).asMicroseconds ();
}

void field_double_mouse (field *fld, v2i choose) {
	if (fld->win || fld->game_over) {
		return;
	}
	MK_C (c, choose);
	if (c->flags & CELL_FLAGS_CLOSED || (!c->val)) {
		return;
	}
	int count = 0;
	FOR_NEIGHBORS (r, choose) {
		MK_C (t, r);
		if (t->flags & CELL_FLAGS_MARK) {
			++count;
		}
	}
	if (count == c->val) {
		FOR_NEIGHBORS (r, choose) {
			MK_C (t, r);
			field_open_cell (fld, r);
		}
	}
}

void field_check_win (field *fld) {
	if (fld->game_over) {
		return;
	}
	int count = 0;
	FOR_2D (v, WWW, HHH) {
		MK_C (c, v);
		if (c->flags & CELL_FLAGS_CLOSED) {
			++count;
		}
	}
	if (count == fld->gp.mines) {
		fld->win = true;
		fld->win_time = 0;
		FOR_2D (v, WWW, HHH) {
			MK_C (c, v);
			if (c->flags & CELL_FLAGS_CLOSED) {
				c->flags |= CELL_FLAGS_MARK;
			}
		}
	}
}

void field_count_mines_left (field *fld) {
	if (fld->game_over) {
		return;
	}
	int count = 0;
	FOR_2D (v, WWW, HHH) {
		MK_C (c, v);
		if (c->flags & CELL_FLAGS_MARK) {
			++count;
		}
	}
	int m_c = 0;
	FOR_2D (v, WWW, HHH) {
		MK_C (c, v);
		if (c->flags & CELL_FLAGS_MINE) {
			++m_c;
		}
	}
	if (m_c != fld->gp.mines) {
		//exit (0);
	}
	fld->mines_left.setCharacterSize(20 * PIX);
	fld->mines_left.setString (std::to_string (fld->gp.mines - count));
	fld->mines_left.setPosition (int(90*PIX - fld->mines_left.getGlobalBounds ().width) + fld->shift, int(7*PIX));
}