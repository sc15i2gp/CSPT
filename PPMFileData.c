#include "PPMFileData.h"

struct ppm_file_data* create_ppm_data(uint width, uint height)
{
	struct ppm_file_data* ppm = new struct ppm_file_data;
	ppm->type = 3;
	ppm->max_val = 255;
	
	ppm->width = width;
	ppm->height = height;
	return ppm;
}

void allocate_colour_data(struct ppm_file_data* file_data)
{
	uint number_of_colour_vals = file_data->width * file_data->height;
	file_data->colour_vals = (uint*)malloc(3 * number_of_colour_vals * sizeof(uint));
}

void destroy_file(struct ppm_file_data* file_data, byte should_dealloc_colours)
{
	if(should_dealloc_colours && file_data->colour_vals) free(file_data->colour_vals);
	free(file_data);
}

void print_to_file(const char* file_name, const char* to_print)
{
	FILE* file = fopen(file_name, "w");
	fprintf(file, "%s", to_print);
	fclose(file);
}

void print_to_ppm(const char* file_name, struct ppm_file_data* file_data)
{
	printf("Printing to ppm: %s...\n", file_name);
	uint file_size = 2+1 //first line 'P3'
			+16+2 //second line width, height
			+8+1 //third line max_value
			+(16 * 3 * file_data->width*file_data->height) //size of each number
			+(3 * file_data->width*file_data->height); //number of whitespace chars
	char* to_print = (char*)malloc(file_size);
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

