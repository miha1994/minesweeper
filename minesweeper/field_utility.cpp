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
        if (fld->time == 0) {
		    fld->a[v].flags |= CELL_FLAGS_CLOSED;
        }
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

#include <Windows.h>

void field_solve (field *fld, bool first) {
	std::vector <v2i> q;
	std::set <v2i> new_cells;
	std::set <v2i> next_cells;
	cell_ar <int> inv (HHH, WWW);
	inv.init (-1);
    int const_mines = 0;
    FOR_2D (v, WWW, HHH) {
        MK_C (c, v);
		TRN_OFF (c->flags, CELL_Q);
        if (c->flags & CELL_P_MINE) {
            ++const_mines;
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
        if (c->flags & CELL_Q) {
			q.push_back (v);
			forset (p, end, v2i, fld->new_digits) {
				if (length (v2f(*p - v) ) < 1.5) {
					new_cells.insert (v);
					break;
				}
			}
			inv[v] = q.size () - 1;
        }
    }
	if (!q.empty ()) {
		std::list <row_for_slau_info> all_eq_slau;
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
					all_eq_slau.push_back (tmp_new);
				}
			}
		}
		if (new_cells.empty ()) {
			first = false;
		}
		while (!new_cells.empty ()) {
			next_cells.clear ();
			std::list <row_for_slau_info> rl;
			std::set <int> in;
			forset (r, end, v2i, new_cells) {
				in.insert (inv[*r]);
			}
			forlist (p, end, row_for_slau_info, all_eq_slau) {
				bool ins = false;
				forset (r, end, v2i, new_cells) {
                    if (p->non_zero_elements.find (inv[*r]) != p->non_zero_elements.end ()) {
						ins = true;
						break;
					}
                    if (p->known_elements.find (inv[*r]) != p->known_elements.end ()) {
						ins = true;
						break;
					}
				}
				if (ins) {
					rl.push_back (*p);
					in.insert (p->non_zero_elements.begin (), p->non_zero_elements.end ());
				}
			}
            if (!first) {
                forset (r, end, v2i, new_cells) {
                    in.erase (inv[*r]);
			    }
            }
            slau_1 sl_1 (rl.size (), in.size (), fld->gp.mines - const_mines);
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
			if (sl_2.h) {
				sl_2.solve ();
			
				v2i tmp_p;
				FOR (i, sl_2.h) {
					tmp_p = sl_2.link_h[i];
					if (GET_F_STATE (sl_2.inf_h[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ONE)) {
						if (!GET_F_STATE (sl_2.inf_h[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ZERO)) {
							MK_C (c, tmp_p);
							if (c->flags & CELL_Q) {
								c->flags &= ~CELL_Q;
								c->flags |= CELL_P_MINE;
								if (new_cells.find (tmp_p) == new_cells.end ()) {
									next_cells.insert (tmp_p);
								}
							}
						}
					} else {
						MK_C (c, tmp_p);
						if (c->flags & CELL_Q) {
							c->flags &= ~CELL_Q;
							c->flags |= CELL_P_SAFE;
							if (new_cells.find (tmp_p) == new_cells.end ()) {
								next_cells.insert (tmp_p);
							}
						}
					}
				}
		
				FOR (i, sl_2.w_A_with_b - 1) {
					tmp_p = sl_2.link_w[i];
					if (GET_F_STATE (sl_2.inf_w[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ONE)) {
						if (!GET_F_STATE (sl_2.inf_w[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ZERO)) {
							MK_C (c, tmp_p);
							if (c->flags & CELL_Q) {
								c->flags &= ~CELL_Q;
								c->flags |= CELL_P_MINE;
								if (new_cells.find (tmp_p) == new_cells.end ()) {
									next_cells.insert (tmp_p);
								}
							}
						}
					} else {
						MK_C (c, tmp_p);
						if (c->flags & CELL_Q) {
							c->flags &= ~CELL_Q;
							c->flags |= CELL_P_SAFE;
							if (new_cells.find (tmp_p) == new_cells.end ()) {
								next_cells.insert (tmp_p);
							}
						}
					}
				}
				new_cells = next_cells;
                
				if (!new_cells.empty ()) {
					all_eq_slau.clear ();
					FOR_2D (v, WWW, HHH) {
						MK_C (c, v);
						if (!(c->flags & CELL_FLAGS_CLOSED)) {
                            tmp_new.non_zero_elements.clear ();
                            tmp_new.known_elements.clear ();
                            tmp_new.right_b = c->val;
							FOR_NEIGHBORS (r, v) {
								if (POINT_IN_RECT (r, 0, 0, WWW, HHH)) {
									MK_C (cr, r);
									if (cr->flags & CELL_FLAGS_CLOSED) {
										if (cr->flags & CELL_P_MINE) {
                                            --tmp_new.right_b;
                                            if (inv[r] >= 0) {
                                                tmp_new.known_elements.insert (inv[r]);
                                            }
										}
										if (cr->flags & CELL_Q) {
                                            tmp_new.non_zero_elements.insert (inv[r]);
										}
                                        if (cr->flags & CELL_P_SAFE) {
                                            if (inv[r] >= 0) {
                                                tmp_new.known_elements.insert (inv[r]);
                                            }
                                        }
									}
								}
							}
                            if (!tmp_new.non_zero_elements.empty ()) {
								all_eq_slau.push_back (tmp_new);
							}
						}
					}
                    first = false;
				}
			} else {
				new_cells = next_cells;
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
				} else {
                    if ((c->flags & CELL_FLAGS_CLOSED) && !(c->flags & CELL_Q)) {
					    ++free_count;
				    }
                    TRN_OFF (c->flags, CELL_FLAGS_MINE);
                }
			}

            std::vector <v2i> q;
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
			        inv[v] = q.size () - 1;
                }
            }

            std::list <row_for_slau_info> all_eq_slau;
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
					    all_eq_slau.push_back (tmp_new);
				    }
			    }
		    }

            slau_1 sl_1 (all_eq_slau.size (), q.size (), fld->gp.mines - const_mines);
			int i = 0;
            forlist (p, end, row_for_slau_info, all_eq_slau) {
				forset (r, end, int, p->non_zero_elements) {
					sl_1.A_b[i][*r] = 1;
				}
				sl_1.A_b[i][sl_1.w_A_with_b - 1] = p->right_b;
				++i;
			}
            sl_1.link = q;

			slau_2 sl_2 = sl_1.solve ();
			if (sl_2.h) {
                sl_2.one_solution_is_enough = true;
				sl_2.solve ();
			
				v2i tmp_p;
				FOR (i, sl_2.h) {
					tmp_p = sl_2.link_h[i];
					if (GET_F_STATE (sl_2.inf_h[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ONE)) {
						MK_C (c, tmp_p);
                        TRN_ON (c->flags, CELL_FLAGS_MINE);
					}
				}
		
				FOR (i, sl_2.w_A_with_b - 1) {
					tmp_p = sl_2.link_w[i];
					if (GET_F_STATE (sl_2.inf_w[i], SLAU_ELEM_INFO_FLAG_CAN_BE_ONE)) {
						MK_C (c, tmp_p);
                        TRN_ON (c->flags, CELL_FLAGS_MINE);
                    }
                }
            }

            std::list <v2i> fr_c;
			FOR_2D (v, WWW, HHH) {
				MK_C (c, v);
				if ((c->flags & CELL_FLAGS_CLOSED) && !((c->flags & CELL_P_MINE) || (c->flags & CELL_P_SAFE) || (c->flags & CELL_Q)) && v != choose) {
					fr_c.push_back (v);
				}
            }

			fld->a[choose].flags = 0;
			const_mines = 0;
			FOR_2D (v, WWW, HHH) {
				MK_C (c, v);
                if ((c->flags & CELL_P_MINE) || (c->flags & CELL_FLAGS_MINE)) {
                    c->flags |= CELL_FLAGS_MINE;
					++const_mines;
				}
			}
			field_make (fld, &fr_c, fld->gp.mines - const_mines);
		} else {
			fld->game_over = true;
			return;
		}
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
	
	fld->new_digits.clear ();
	FOR_NEIGHBORS (r, choose) {
		MK_C (c, r);
		if (! (c->flags & CELL_FLAGS_CLOSED)) {
			fld->new_digits.insert (r);
		}
	}
	FOR_2D(v, WWW, HHH) {
		if (ca[v].val) {
			MK_C (c, v);
			c->flags &= ~CELL_FLAGS_CLOSED;
			fld->new_digits.insert (v);
			FOR_NEIGHBORS (r, v) {
				MK_C (c1, r);
				if (! (c1->flags & CELL_FLAGS_CLOSED)) {
					fld->new_digits.insert (r);
				}
			}
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
		fld->wm.state = 0;
		/*
		FOR_2D (v, WWW, HHH) {
			MK_C (c, v);
			if (c->flags & CELL_FLAGS_CLOSED) {
				c->flags |= CELL_FLAGS_MARK;
			}
		}
		*/
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
	if (fld->win) {
		count = fld->gp.mines;
	}
	fld->mines_left.setCharacterSize(20 * PIX);
	fld->mines_left.setString (std::to_string (fld->gp.mines - count));
	fld->mines_left.setPosition (int(90*PIX - fld->mines_left.getGlobalBounds ().width) + fld->shift, int(7*PIX));
}