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
		if (fld->history_is_writing == false) {
			nxt = fld->hist.get_rand_n (free_cells->size ());
		} else {
			fld->hist.push_rand_n (nxt);
		}
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

inline void field_swag_upd (field *fld) {
	if (fld->swag_time < 2) {
		if (fld->swag_time > 0) {
			fld->swag_power += (3 - fld->swag_time) * 0.1;
			fld->swag_power = Min (1, fld->swag_power);
		}
	} else {
		fld->swag_power = 0;
	}
	fld->swag_time = 0;
}

void field_solve (field *fld, bool first) {
	std::vector <v2i> q;
	std::set <v2i> new_cells;
	std::set <v2i> next_cells;
	cell_ar <int> inv (HHH, WWW);
	inv.init (-1);
    int const_mines = 0, free_count = 0;
    FOR_2D (v, WWW, HHH) {
        MK_C (c, v);
		TRN_OFF (c->flags, CELL_Q);
        if (c->flags & CELL_P_MINE) {
            ++const_mines;
        }
		if ((c->flags & CELL_FLAGS_CLOSED) && !((c->flags & CELL_P_MINE) || (c->flags & CELL_P_SAFE) || (c->flags & CELL_Q))) {
			++free_count;
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
            slau_1 sl_1 (rl.size (), in.size (), fld->gp.mines - const_mines, fld->gp.mines - const_mines - free_count);
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
					sl_1.A_b[i][num[*r]] = r_n (1);
				}
				sl_1.A_b[i][sl_1.w_A_with_b - 1] = r_n (p->right_b);
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

void field_open_cell (field *fld, v2i choice) {
	if (!POINT_IN_RECT (choice, 0, 0, WWW, HHH) ) {
		return;
	}
	if (!(fld->a[choice].flags & CELL_FLAGS_CLOSED) || (fld->a[choice].flags & CELL_FLAGS_MARK)) {
		return;
	}
	if (fld->history_is_writing) {
		fld->hist.push_left_click (choice);
	}
	if (fld->a[choice].flags & CELL_FLAGS_MINE) {
        if (fld->no_moves && fld->b_switch_mine_moving_ability) {
            if ((fld->a[choice].flags & CELL_Q)) {
				auto scrn = fld->a;
				try {
					fld->a[choice].flags &= ~CELL_Q;
					fld->a[choice].flags |= CELL_P_SAFE;

					int free_count = 0;
					int const_mines = 0;
					FOR_2D (v, WWW, HHH) {
						MK_C (c, v);
						if ((c->flags & CELL_FLAGS_CLOSED) && (c->flags & CELL_P_MINE)) {
							++const_mines;
						} else {
							if ((c->flags & CELL_FLAGS_CLOSED) && !(c->flags & CELL_Q) && v != choice) {
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

					slau_1 sl_1 (all_eq_slau.size (), q.size (), fld->gp.mines - const_mines, fld->gp.mines - const_mines - free_count);
					int i = 0;
					forlist (p, end, row_for_slau_info, all_eq_slau) {
						forset (r, end, int, p->non_zero_elements) {
							sl_1.A_b[i][*r] = r_n (1);
						}
						sl_1.A_b[i][sl_1.w_A_with_b - 1] = r_n (p->right_b);
						++i;
					}
					sl_1.link = q;

					slau_2 sl_2 = sl_1.solve ();
					if (sl_2.h) {
						sl_2.one_solution_is_enough = true;
						if (!sl_2.solve ()) {
							throw 1;
						}

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
						if ((c->flags & CELL_FLAGS_CLOSED) && !((c->flags & CELL_P_MINE) || (c->flags & CELL_P_SAFE) || (c->flags & CELL_Q)) && v != choice) {
							fr_c.push_back (v);
						}
					}

					fld->a[choice].flags = 0;
					const_mines = 0;
					FOR_2D (v, WWW, HHH) {
						MK_C (c, v);
						if ((c->flags & CELL_FLAGS_CLOSED) && ((c->flags & CELL_P_MINE) || (c->flags & CELL_FLAGS_MINE))) {
							c->flags |= CELL_FLAGS_MINE;
							++const_mines;
						}
					}
					field_make (fld, &fr_c, fld->gp.mines - const_mines);
					fld->reallocation_count.set_inf_value ( fld->reallocation_count.get_inf_value () + 1);
					fld->reallocation_time = 0;
				}
				catch (int) {
					fld->a = scrn;
					fld->fail (choice);
					return;
				}
            } else if (!(fld->a[choice].flags & CELL_P_MINE)) {
                FOR_2D (v, WWW, HHH) {
				    MK_C (c, v);
                    if (c->flags & CELL_Q) {
                        fld->fail (choice);
                        return;
                    }
                }
			    fld->a[choice].flags |= CELL_P_SAFE;

			    int free_count = 0;
			    int const_mines = 0;
			    FOR_2D (v, WWW, HHH) {
				    MK_C (c, v);
				    if ((c->flags & CELL_FLAGS_CLOSED) && (c->flags & CELL_FLAGS_CLOSED) && (c->flags & CELL_P_MINE)) {
					    ++const_mines;
				    } else {
                        if ((c->flags & CELL_FLAGS_CLOSED) && !(c->flags & CELL_Q)) {
					        ++free_count;
				        }
                        TRN_OFF (c->flags, CELL_FLAGS_MINE);
                    }
			    }

                std::list <v2i> fr_c;
			    FOR_2D (v, WWW, HHH) {
				    MK_C (c, v);
				    if ((c->flags & CELL_FLAGS_CLOSED) && !((c->flags & CELL_P_MINE) || (c->flags & CELL_P_SAFE) || (c->flags & CELL_Q)) && v != choice) {
					    fr_c.push_back (v);
				    }
                }

			    field_make (fld, &fr_c, fld->gp.mines - const_mines);
                fld->reallocation_count.set_inf_value ( fld->reallocation_count.get_inf_value () + 1);
                fld->reallocation_time = 0;
            } else {
				fld->fail (choice);
                return;
            }
        } else {
            fld->fail (choice);
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
	if (!fld->a[choice].val) {
		dots[sw].push_back (choice);
	}
	ca[choice].val = 1;
	ca[choice].flags = 1;

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
	FOR_NEIGHBORS (r, choice) {
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
			if ( v != choice) {
				force_cell fc0 (v, my_clr_s[color_theme].unknown, CLR_BLIND (my_clr_s[color_theme].safe), 0.1, 0.03 * length (v2f(v - choice)));
				force_cell fc1 (v, my_clr_s[color_theme].unknown, my_clr_s[color_theme].unknown, 0.03 * length (v2f(v - choice)));
				fld->fc_l.push_back (fc0);
				fld->fc_l.push_back (fc1);
			}
		}
	}
	sf::Clock timer;
    timer.restart ();
	field_swag_upd (fld);
	field_solve (fld);
	fld->mkr +=	(timer.getElapsedTime ()).asMicroseconds ();
}

void field_double_mouse (field *fld, v2i choice) {
	if (fld->win () || fld->game_over) {
		return;
	}
	MK_C (c, choice);
	if (c->flags & CELL_FLAGS_CLOSED || (!c->val)) {
		return;
	}
	int count = 0;
	FOR_NEIGHBORS (r, choice) {
		MK_C (t, r);
		if (t->flags & CELL_FLAGS_MARK) {
			++count;
		}
	}
	if (count == c->val) {
		FOR_NEIGHBORS (r, choice) {
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
		fld->set_win (true);
		fld->win_time = 0;
		fld->wm.state = 0;
		SV_info s;
		get_save_status (s);
		round_results rs;
		rs.hints_used = fld->reallocation_count.get_inf_value ();
		rs.time = fld->time;
		s.statistic[fld->gp].wins.insert (rs);
		s.statistic[fld->gp].wins_count++;
		set_save_status (s);
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
	if (fld->win ()) {
		count = fld->gp.mines;
	}
	fld->mines_left.setCharacterSize(20 * PIX);
	fld->mines_left.setString (std::to_string (fld->gp.mines - count));
	fld->mines_left.setPosition (int(90*PIX - fld->mines_left.getGlobalBounds ().width) + fld->shift, int(7*PIX));
}

void field_ch_pix (field *fld) {
	fld->menu.pos = v2f (6, 3.5) * PIX + v2f (fld->shift, 0);
	fld->menu.act = &fld->menu_a[PX_1];
	fld->menu.spr = &fld->menu_na[PX_1];
	float scale = (fld->pix / PX_96) * (28.0 / 30);
	fld->menu.act->itself.setScale (scale, scale);
	fld->menu.spr->itself.setScale (scale, scale);

    fld->hint.pos = v2f ((MY_WIND_WIDTH / 2 + 18) * PIX + fld->shift, 3.5 * PIX);
	fld->hint.act = &fld->hint_a[PX_1];
	fld->hint.spr = &fld->hint_na[PX_1];
	fld->hint.act->itself.setScale (scale, scale);
	fld->hint.spr->itself.setScale (scale, scale);

	fld->show_results.pos = v2f (fld->shift, fld->vshift) + v2f(!v2i(6, 43) + v2f (0, 0)*32.0f) * PIX;
	fld->show_results.act = fld->show_results.spr = &fld->spr_show_results[PX_1];
	fld->show_results.act->itself.setScale (fld->pix / PX_96, fld->pix / PX_96);

	FOR (i, 4) {
		fld->sm.spr[i][PX_1].itself.setPosition ((MY_WIND_WIDTH / 2 - 16) * PIX + fld->shift, 3.5 * PIX);
		fld->sm.spr[i][PX_1].itself.setScale ((fld->pix / PX_96) * (28.0 / 30), (fld->pix / PX_96) * (28.0 / 30));
	}
	fld->wm.spr[PX_1].itself.setScale ((fld->pix / PX_96), (fld->pix / PX_96));

	fld->mines_left.setCharacterSize (20 * PIX);
	fld->mines_left.setPosition (int(90*PIX - fld->mines_left.getGlobalBounds ().width) + fld->shift , int(7*PIX));
    fld->reallocation_count->setCharacterSize (20 * PIX);
    fld->reallocation_count.set_pos ((MY_WIND_WIDTH / 2 + 18 + 40) * PIX + fld->shift, 13 * PIX);
	fld->sec.setCharacterSize(20 * PIX);
	fld->sec.setPosition(int((MY_WIND_WIDTH - 12)*PIX - fld->sec.getGlobalBounds ().width) + fld->shift,int(6.4*PIX));	
	fld->bg.setSize (v2f(fld->inside_w, (HHH*32 + 12) * PIX + (fld->vshift <= 1 ? 37.0 * PIX : 0)));
	fld->bg.setPosition (fld->shift, fld->vshift + (fld->vshift > 1 ? 37.0 * PIX : 0));
}

void field_reset (field *fld) {
	for (int i = -1; i <= fld->gp.height; ++i) {
		for (int j = -1; j <= fld->gp.width; ++j) {
			fld->a[i][j].flags = CELL_FLAGS_CLOSED;
			fld->a[i][j].val = 0;
            fld->a[i][j].open_time = 0;
		}
	}
	fld->swag_power = 0;
	fld->swag_time = 0;
	fld->empty = true;
	fld->game_over = false;
	fld->set_win (false);
	fld->time = 0.0f;
    fld->reallocation_count.set_inf_value (0);
	fld->mkr = 0;
	fld->no_moves = false;
	fld->state = FIELD_STATE_INTRO;
	fld->hist.clear ();

	if (fld->load ()) {
		fld->empty = false;
		fld->state = FIELD_STATE_IN_GAME;
	}
}