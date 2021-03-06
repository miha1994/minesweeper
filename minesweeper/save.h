#pragma once
#include "main_header.h"
#include "table_stat.h"

struct SV_info {
	int First;
	game_parameters Last_mode;
	int Col_theme;
	int Num_of_templates;
    int Mine_moving_ability;
    int Safe_opening_ability;
	int q_is_enabled;
	vec <game_parameters> templates;
	std::map <game_parameters, table_stat> statistic;
};

void get_save_status (SV_info &s_info);
void set_save_status (SV_info &s_info);