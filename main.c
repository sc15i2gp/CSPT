#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Parse.h"

//TODO: print file_info struct to file
//TODO: Hash function for RGB values
//TODO: Red-black tree implementation of hashmap
//TODO: Create pattern image from input ppm
//	Load pattern symbols into hashmap
//	Create array of pattern symbol hashes with width*height of input file
//	Hashmap of colour to times colour occurs
//	Fill pattern symbol hashes array with hashes corresponding to each pixel's colour in input file
//	Convert pattern symbol hashes array to uint array to output to ppm file

int main(int argc, char** argv)
{
	if(argc <= 1)
	{
		printf("Error: No file specified!\n");
		return 1;
	}
	else
	{
		struct file_info* file_data = process_file(argv[1]);
		if(file_data)
		{
			byte b = file_data->type == 3;
			print_to_ppm("jotaro_copy.ppm", file_data);
			destroy_file(file_data);
			return b;
		}
		else return -1;
	}
}
