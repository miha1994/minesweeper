#include "func_pointers.h"
#include "o_control.h"
#include "o_button.h"
#include "o_field.h"

std_obj_functions *fp_array [] = {
	&control_f,			//0
	&field_f,		    //1
	&button_f,			//2
};