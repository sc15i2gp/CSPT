#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned int uint;

struct file_info
{
	byte 	type;
	uint 	width;
	uint 	height;
	uint 	max_val;
	uint* 	colour_vals;
};

void allocate_colour_data(struct file_info*);
void destroy_file(struct file_info*, byte should_dealloc_colours = 1);
void print_to_ppm(const char* file_name, struct file_info*);
#endif
