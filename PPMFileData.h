#ifndef PPM_FILE_DATA_H
#define PPM_FILE_DATA_H

//PPM standard values:
//	Type = 3 (says that the file is a ppm)
//	max_val = 255 (ppms will always store info as raw ascii)

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

struct ppm_file_data* create_ppm_data(uint width, uint height);
void allocate_colour_data(struct ppm_file_data*);
void destroy_file(struct ppm_file_data*, byte should_dealloc_colours = 1);
void print_to_ppm(const char* file_name, struct ppm_file_data*);
void print_to_file(const char* path, const char* to_print);
#endif
