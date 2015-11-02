#include "save.h"

#define SZ1		sizeof (int), 1, f

void get_save_status (SV_info *s_info) {
	FILE *f = fopen ("assets/inf/MS", "rb");
	fread (&s_info->First, SZ1);
	fread (&s_info->Last_mode, sizeof (game_parameters), 1, f);
	fread (&s_info->Col_theme, SZ1);
	fread (&s_info->Num_of_templates, SZ1);
	s_info->templates.clear ();
	s_info->templates.push_back (game_parameters ( 9, 9,10));
	s_info->templates.push_back (game_parameters (16,16,40));
	s_info->templates.push_back (game_parameters (16,30,99));
	game_parameters gp;
	FOR (i, s_info->Num_of_templates) {
		fread (&gp.height, SZ1);
		fread (&gp.width, SZ1);
		fread (&gp.mines, SZ1);
		s_info->templates.push_back (gp);
	}
	fclose (f);
}

void set_save_status (SV_info s_info) {
	FILE *f = fopen ("assets/inf/MS", "wb");
	fwrite (&s_info.First, SZ1);
	fwrite (&s_info.Last_mode, sizeof (game_parameters), 1, f);
	fwrite (&s_info.Col_theme, SZ1);
	s_info.Num_of_templates = s_info.templates.size () - 3;
	fwrite (&s_info.Num_of_templates, SZ1);
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