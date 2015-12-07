#pragma once
#include "o_field.h"

void field_make (field *fld, std::list <v2i> *free_cells, int mines);
void field_solve (field *fld);
void field_open_cell (field *fld, v2i choose);
void field_double_mouse (field *fld, v2i choose);
void field_check_win (field *fld);
void field_count_mines_left (field *fld);