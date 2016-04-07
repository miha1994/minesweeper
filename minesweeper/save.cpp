#include "save.h"

#define SZ1				sizeof (int), 1, f
#define write(f, what)	fwrite (&(what), sizeof(what), 1, f)
#define read(f, what)	fread (&(what), sizeof(what), 1, f)

void get_save_status (SV_info &s_info) {
	s_info.templates.clear ();
	s_info.templates.push_back (game_parameters ( 9, 9,10));
	s_info.templates.push_back (game_parameters (16,16,40));
	s_info.templates.push_back (game_parameters (16,30,99));

	FILE *f = fopen ("assets/inf/MS", "rb");
	if (!f) {
		s_info.First = true;
		s_info.Col_theme = 0;
		s_info.Last_mode.width = s_info.Last_mode.height = 9;
		s_info.Last_mode.mines = 10;
		s_info.Mine_moving_ability = true;
		s_info.Safe_opening_ability = false;
		s_info.q_is_enabled = false;
		s_info.Num_of_templates = 0;
		s_info.statistic.clear ();
		return;
	}
	fread (&s_info.First, SZ1);
	fread (&s_info.Last_mode, sizeof (game_parameters), 1, f);
	fread (&s_info.Col_theme, SZ1);
    fread (&s_info.Num_of_templates, SZ1);
    read (f, s_info.Mine_moving_ability);
    read (f, s_info.Safe_opening_ability);
	read (f, s_info.q_is_enabled);
	
	game_parameters gp;
    read (f, s_info.Num_of_templates);
	FOR (i, s_info.Num_of_templates) {
		fread (&gp.height, SZ1);
		fread (&gp.width, SZ1);
		fread (&gp.mines, SZ1);
		s_info.templates.push_back (gp);
	}

	table_stat ts;
	round_results rr;
	s_info.statistic.clear ();
	int n;
	if (!read (f, n)) {
		n = 0;
	}
	FOR (i, n) {
		fread (&gp.height, SZ1);
		fread (&gp.width, SZ1);
		fread (&gp.mines, SZ1);

		read (f, ts.fails_count);
		read (f, ts.wins_count);
		int m;
		read (f, m);
		FOR (j, m) {
			read (f, rr.time);
			read (f, rr.hints_used);
			if (ts.wins.size () < 11) {
				ts.wins.insert (rr);
			}
		}
		s_info.statistic[gp] = ts;
		ts.wins.clear ();
	}
	fclose (f);
}

void set_save_status (SV_info &s_info) {
	FILE *f = fopen ("assets/inf/MS", "wb");
	fwrite (&s_info.First, SZ1);
	fwrite (&s_info.Last_mode, sizeof (game_parameters), 1, f);
	fwrite (&s_info.Col_theme, SZ1);
    fwrite (&s_info.Mine_moving_ability, SZ1);
	s_info.Num_of_templates = s_info.templates.size () - 3;
    write (f, s_info.Mine_moving_ability);
    write (f, s_info.Safe_opening_ability);
	write (f, s_info.q_is_enabled);
    write (f, s_info.Num_of_templates);
	int number_of_skips = 3;
	forvector (p, end, game_parameters, s_info.templates) {
		if (number_of_skips) {
			number_of_skips--;
			continue;
		}
		fwrite (&p->height, SZ1);
		fwrite (&p->width, SZ1);
		fwrite (&p->mines, SZ1);
	}

	int n = s_info.statistic.size ();
	/*
	write (f, n = 0);
	fclose (f);
	return;
	*/
	write (f, n);
	forstl (p, end, s_info.statistic) {
		write (f, p->first.height);
		write (f, p->first.width);
		write (f, p->first.mines);

		write (f, p->second.fails_count);
		write (f, p->second.wins_count);
		n = p->second.wins.size ();
		write (f, n);
		forstl (p1, end, p->second.wins) {
			write (f, p1->time);
			write (f, p1->hints_used);
		}
	}
	fclose (f);
}

/*
void get_save_status (int *number_of_beaten_nights) {
	FILE *f = fopen ("assets/inf/MS", "rb");
	fread (number_of_beaten_nights, sizeof (int), 1, f);
	fclose (f);
}

void set_save_status (int number_of_beaten_nights) {
	FILE *f = fopen ("assets/inf/MS", "wb");
	fwrite (&number_of_beaten_nights, sizeof (int), 1, f);
	fclose (f);
}
*/