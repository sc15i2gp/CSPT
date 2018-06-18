#include "FileInfo.h"

void allocate_colour_data(struct file_info* file_data)
{
	uint number_of_colour_vals = file_data->width * file_data->height;
	printf("Allocating %d pixles of uints\n", number_of_colour_vals);
	file_data->colour_vals = malloc(3 * number_of_colour_vals * sizeof(uint));
}

void destroy_file(struct file_info* file_data)
{
	if(file_data->colour_vals) free(file_data->colour_vals);
	free(file_data);
}

void print_to_ppm(const char* file_name, struct file_info* file_data)
{
	printf("Printing to ppm: %s...\n", file_name);
	uint file_size = 2+1 //first line 'P3'
			+16+2 //second line width, height
			+8+1 //third line max_value
			+(16 * 3 * file_data->width*file_data->height) //size of each number
			+(3 * file_data->width*file_data->height); //number of whitespace chars
	char* to_print = malloc(file_size);
	char* current_char = to_print;
	*current_char = 'P';
	current_char++;
	current_char += sprintf(current_char, "%u\n", file_data->type);
	current_char += sprintf(current_char, "%u %u\n", file_data->width, file_data->height);
	current_char += sprintf(current_char, "%u\n", file_data->max_val);

	for(uint i = 0; i < file_data->height; i++)
	{
		for(uint j = 0; j < file_data->width * 3; j++)
		{
			uint current_val = *(file_data->colour_vals + (i*3*file_data->width) + j);
			current_char += sprintf(current_char, "%u\n", current_val);
		}
	}
	print_to_file(file_name, to_print);
	free(to_print);
	printf("Done printing\n");
}

