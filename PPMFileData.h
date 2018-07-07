#ifndef PPM_FILE_DATA_H
#define PPM_FILE_DATA_H

#include <stdio.h>
#include <stdlib.h>

	typedef unsigned char byte;
	typedef unsigned int uint;

struct ppm_file_data
{
	byte 	type;
	uint 	width;
	uint 	height;
	uint 	max_val;
	uint* 	colour_vals;
};

void allocate_colour_data(struct ppm_file_data*);
void destroy_file(struct ppm_file_data*, byte should_dealloc_colours = 1);
void print_to_ppm(const char* file_name, struct ppm_file_data*);
#endif
