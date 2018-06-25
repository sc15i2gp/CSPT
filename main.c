#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Parse.h"
#include "Map.h"

//TODO: Create pattern image from input ppm
//
//	Create file_data* for output pattern
//	Calculate new size of file_data* colour_vals* and allocate
//	For each row of pixels in input file:
//		For each row of pixels in pattern symbol (i = 0 to 15):
//			For each pixel in a row of input ppm
//				Copy row[i] pixel values of pattern` symbol
//

const char* symbol_prefix = "Symbols/S_";
const char* symbol_extension = ".ppm";

#define NUM_SYMBOLS 31


void load_symbol(const char* symbol_path, struct file_info** symbol_loc)
{
	*symbol_loc = process_file(symbol_path);
}

struct file_info** load_symbols()
{
	struct file_info** symbols = new struct file_info*[NUM_SYMBOLS];
	
	for(uint i = 0; i < NUM_SYMBOLS; i++)
	{
		char* path = new char[strlen(symbol_prefix) + strlen(symbol_extension) + 2];
		sprintf(path, "%s%02d%s", symbol_prefix, i, symbol_extension);
		symbols[i] = process_file(path);
		delete[] path;
	}
	return symbols;
}

void destroy_symbols(struct file_info** symbols)
{
	for(uint i = 0; i < NUM_SYMBOLS; i++)
	{
		destroy_file(symbols[i]);
	}
	delete[] symbols;
}

void print_colour_pair(struct kv_pair* p, uint colour)
{
	const char* c = (colour == RED) ? "RED" : "BLACK";
	printf("{ Key: %06x | Value: %04d | Colour: %s }\n", p->key, p->value, c);
}

void print_colour_map(struct rb_tree* t)
{
	printf("===========================================\n");
	execute_for_each_pair(t, print_colour_pair);
	printf("Number of colours = %d\n", count_nodes(t));
	printf("===========================================\n\n");
}
void assert_node(struct node* n, uint key, uint colour)
{
	printf("Asserting key %x found key %x\n", key, n->pair.key);
	assert(n->pair.key == key);
	assert(n->colour == colour);
}

void test_map()
{
	printf("Testing map...\n");
	struct rb_tree* t = create_rb_tree();

	uint to_insert_first = 0xffffff;
	uint to_insert_second = 0;
	uint to_insert_third = 0x516d87;
	uint to_insert_fourth = 0x616180;
	uint to_insert_fifth = 0x738caa;	
	uint to_insert_sixth = 0xbbd0da;
	uint to_insert_seventh = 0x3d0067;
	uint to_insert_eighth = 0xffb000;

	(*t)[to_insert_first];
	(*t)[to_insert_second];
	(*t)[to_insert_third];
	(*t)[to_insert_fourth];
	(*t)[to_insert_fifth];
	(*t)[to_insert_sixth];
	(*t)[to_insert_seventh];
	(*t)[to_insert_eighth];

	uint to_insert_ninth = 0xdda66b;
	(*t)[to_insert_ninth];

	assert_node(t->root->left_child, to_insert_third, RED);
	assert_node(t->root->left_child->left_child, to_insert_second, BLACK);
	assert_node(t->root->left_child->right_child, to_insert_fourth, BLACK);

	print_colour_map(t);
	destroy_rb_tree(t);
	printf("Map testing successful\n");
}

uint hash_RGB(uint R, uint G, uint B)
{
	//Set result to be 0xRRGGBB
	uint result = 0x000000;
	R = (R << 16);
	G = (G << 8);
	B = (B << 0);
	result = R | G | B;
	return result;
}

void sum_stitch_value(struct kv_pair* p, uint* count)
{
	*count += p->value;
}

void print_stitch_count(struct rb_tree* t)
{
	uint count = 0;
	execute_for_each_pair(t, &count, sum_stitch_value);
	printf("Total number of stitches = %d\n", count);
}

struct rb_tree* load_colour_map(struct file_info* file_data)
{
	struct rb_tree* colour_map = create_rb_tree();
	uint i_max = file_data->width * file_data->height;
	for(uint i = 0; i < i_max; i++)
	{
		uint* px_RGB = file_data->colour_vals + 3*i;
		uint key = hash_RGB(*px_RGB, *(px_RGB + 1), *(px_RGB + 2));
		(*colour_map)[key];
	}
	return colour_map;
}

void count_colours_in_file(struct file_info* file_data)
{
	printf("Counting...\n");
	struct rb_tree* colour_map = load_colour_map(file_data);
	uint prev_RGB = -1;
	uint i_max = file_data->width * file_data->height;
	for(uint i = 0; i < i_max; i++)
	{
		uint* px_RGB = file_data->colour_vals + 3*i;
		uint key = hash_RGB(*px_RGB, *(px_RGB + 1), *(px_RGB + 2));
		(*colour_map)[key]++;
	}
	printf("Counted\n");
	print_colour_map(colour_map);
	print_stitch_count(colour_map);
	destroy_rb_tree(colour_map);
}

void test_symbols()
{
	struct file_info** symbols = load_symbols();

	for(uint i = 0; i < NUM_SYMBOLS; i++)
	{
		char* path = new char[2 + 4];
		sprintf(path, "%02d%s", i, ".ppm");
		print_to_ppm(path, symbols[i]);
	}
	destroy_symbols(symbols);
}

struct rb_tree* map_src_RGB_to_symbols(struct file_info* src_image)
{
	struct rb_tree* colour_map = create_rb_tree();
	uint i_max = src_image->width * src_image->height;
	uint symbol_index = 1;
	for(uint i = 0; i < i_max; i++)
	{
		uint* px_RGB = src_image->colour_vals + 3*i;
		uint key = hash_RGB(*px_RGB, *(px_RGB + 1), *(px_RGB + 2));
		//White should always map to the first symbol
		if(!is_key_in_tree(colour_map, key))
		{
			if(key == 0xffffff) (*colour_map)[key] = 0;
			else
			{
				(*colour_map)[key] = symbol_index;
				symbol_index++;
			}
		}
	}
	return colour_map;
}

struct file_info* create_pattern_image(struct file_info* src_image)
{
	struct file_info* pattern_image = new struct file_info;
	uint type = 3;
	//One pixel in src corresponds to a 16*16 symbol in output file
	uint width = src_image->width * 16;
	uint height = src_image->height * 16;
	uint max_val = 256;

	pattern_image->type = type; //Set 3 as the default
	pattern_image->width = width;
	pattern_image->height = height;
	pattern_image->max_val = max_val;

	allocate_colour_data(pattern_image);
	return pattern_image;	
}

void populate_pattern_colour_data(struct file_info* src_image, struct file_info* pattern_image, struct rb_tree* colour_map)
{
	struct file_info** ps_list = load_symbols();
	//Transform src => {colour_map} =>  pattern_image
	//For each pixel in pattern_image
	for(uint pat_px_index = 0; pat_px_index < pattern_image->width * pattern_image->height; pat_px_index++)
	{
		uint pat_px_row = (uint)(pat_px_index / pattern_image->width);
		uint pat_px_col = (uint)(pat_px_index % pattern_image->width);

		//Find current pixel's RBG in the source image
		uint src_px_row = (uint) (pat_px_row / 16);
		uint src_px_col = (uint) (pat_px_col / 16);
		uint src_px_index = 3*(src_px_row*src_image->width + src_px_col);
		
		uint* src_px_RGB = src_image->colour_vals + src_px_index;
		uint key = hash_RGB(*src_px_RGB, *(src_px_RGB + 1), *(src_px_RGB + 2));
		struct file_info* symbol = ps_list[(*colour_map)[key]];

		//Get symbol's pixel RGB to put in pat_px_index
		uint symbol_px_row = pat_px_row % 16;
		uint symbol_px_col = pat_px_col % 16;

		uint symbol_px_index = 3*(16*symbol_px_row + symbol_px_col);

		uint* symbol_px_RGB = symbol->colour_vals + symbol_px_index;
		uint* pat_px_RGB = pattern_image->colour_vals + 3*pat_px_index;
		*pat_px_RGB = *symbol_px_RGB;
		*(pat_px_RGB + 1) = *(symbol_px_RGB + 1);
		*(pat_px_RGB + 2) = *(symbol_px_RGB + 2);
	}
	destroy_symbols(ps_list);
}

void create_pattern_from_src(struct file_info* src_image)
{
	count_colours_in_file(src_image);
	
	//Map of hashed RGB => pattern symbol
	struct rb_tree* colour_map = map_src_RGB_to_symbols(src_image);
	
	struct file_info* pattern_image = create_pattern_image(src_image);
	
	populate_pattern_colour_data(src_image, pattern_image, colour_map);

	print_to_ppm("output.ppm", pattern_image);

	destroy_file(pattern_image);
	destroy_rb_tree(colour_map);
	destroy_file(src_image);
}

byte create_pattern(const char* src_image_path)
{
	byte pattern_created = 0;
	struct file_info* src_image = process_file(src_image_path);
	if(src_image)
	{
		create_pattern_from_src(src_image);
		pattern_created = 1;
	}
	return pattern_created;
}

int main(int argc, char** argv)
{
	if(argc <= 1)
	{
		printf("Error: No file specified!\n");
		return 2;
	}
	else
	{
		byte pattern_created = create_pattern(argv[1]);
		return !pattern_created;
	}
}
