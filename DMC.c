#include "DMC.h"

struct rb_tree* create_DMC_floss_map()
{
	struct rb_tree* floss_map = create_rb_tree();
	
	for(uint i = 0; i < FLOSS_COUNT; i++)
	{
		(*floss_map)[DMC_flosses[i].key] = DMC_flosses[i].value;
	}

	return floss_map;
}


