#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Parse.h"

uint hash_RGB(uint R, uint G, uint B)
{
	//Set result to be 0xRRGGBB
	uint result = 0x000000;
	R = (R << 24);
	G = (G << 16);
	B = (B << 8);
	result = R | G | B;
	return result;
}

//TODO: print file_info struct to file
//TODO: Hash function for RGB values
//TODO: Implement map struct
//		hashed RGB = > int which points to symbol
//TODO: List of pairs of ints => pattern symbol file data
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
			uint hash = hash_RGB(0xFF, 0xA5, 0x10);
			printf("hashed value = %08x\n", hash);
			destroy_file(file_data);
			return !b;
		}
		else return -1;
	}
}
