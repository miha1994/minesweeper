#include "func_pointers.h"
#include "o_control.h"
#include "o_button.h"

std_obj_functions *fp_array [] = {
	&control_f,			//0
	NULL,			    //1
	&button_f,			//2
};