#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Parse.h"
#include "Map.h"

//TODO: Create pattern image from input ppm
//	Load list of pattern symbols (list of file_data*)
//	
//	Create colour map
//	For each pixel in input file:
//		If pixel's colour hash is not in colour map
//			Get next pattern symbol not in use
//			Set to in use
//			Make pixel's colour hash map to pattern symbol
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

byte create_pattern(const char* src_image_path)
{
	byte pattern_created = 0;
	struct file_info** ps_list = load_symbols();
	byte is_symbol_in_use[NUM_SYMBOLS];
	struct file_info* src_image = process_file(src_image_path);
	if(src_image)
	{
		count_colours_in_file(src_image);
		
		//Map of hashed RGB => pattern symbol
		struct rb_tree* colour_symbol_map = load_colour_map(src_image);

		//Construct colour map
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

		print_colour_map(colour_map);

		destroy_rb_tree(colour_symbol_map);
		destroy_file(src_image);
		pattern_created = 1;
	}
	destroy_symbols(ps_list);
	return pattern_created;
}

int main(int argc, char** argv)
{
	test_symbols();
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
