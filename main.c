#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Parse.h"
#include "Map.h"

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

void print_colour_pair(struct kv_pair* p, uint colour)
{
	const char* c = (colour == RED) ? "RED" : "BLACK";
	printf("{ Key: %06x | Value: %04d | Colour: %s }\n", p->key, p->value, c);
}

//TODO: Tidy up map code
//TODO: Create pattern image from input ppm
//	Load pattern symbols into hashmap
//	Create array of pattern symbol hashes with width*height of input file
//	Hashmap of colour to times colour occurs
//	Fill pattern symbol hashes array with hashes corresponding to each pixel's colour in input file
//	Convert pattern symbol hashes array to uint array to output to ppm file

void assert_node(struct node* n, uint key, uint colour)
{
	printf("Asserting key %x found key %x\n", key, n->pair.key);
	assert(n->pair.key == key);
	assert(n->colour == colour);
}

void print_colour_map(struct rb_tree* t)
{
	printf("===========================================\n");
	execute_for_each_pair(t, print_colour_pair);
	printf("===========================================\n\n");
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

void count_colours_in_file(struct file_info* file_data)
{
	struct rb_tree* colour_map = create_rb_tree();
	uint i_max = file_data->width * file_data->height;
	printf("Counting...\n");
	uint prev_RGB = -1;
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

int main(int argc, char** argv)
{
	test_map();
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
			count_colours_in_file(file_data);
			destroy_file(file_data);
			return !b;
		}
		else return -1;
	}
}
