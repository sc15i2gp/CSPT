#include "Pattern.h"

void print_floss_to_stitch_count_map(struct rb_tree* t)
{
	printf("===========================================\n");
	print_kv_pairs(t,"Floss %d has %d stitches\n");
	printf("===========================================\n\n");
}

void print_pattern_info(struct pattern_info* p_info)
{
	printf("\n================================================\n");
	printf("Pattern output:\n");
	printf("Width = %d stitches\n", p_info->width_in_stitches);
	printf("Height = %d stitches\n", p_info->height_in_stitches);
	printf("Total = %d stitches\n", p_info->number_of_stitches);
	printf("Number of flosses = %d\n", count_nodes(p_info->floss_to_stitch_count_map));
	print_floss_to_stitch_count_map(p_info->floss_to_stitch_count_map);	
	printf("\n================================================\n");
}


void set_floss_to_symbol_pair(struct node* n, struct rb_tree* colour_to_floss_map, struct rb_tree* floss_to_symbol_map)
{
	if(n)
	{
		struct kv_pair p = n->pair;
		set_floss_to_symbol_pair(n->left_child, colour_to_floss_map, floss_to_symbol_map);
		(*floss_to_symbol_map)[(*colour_to_floss_map)[p.key]] = p.value;
		set_floss_to_symbol_pair(n->right_child, colour_to_floss_map, floss_to_symbol_map);
	}	
}

struct rb_tree* create_floss_to_symbol_map(struct rb_tree* colour_to_symbol_map, struct rb_tree* colour_to_floss_map)
{
	struct rb_tree* floss_to_symbol_map = create_rb_tree();
	set_floss_to_symbol_pair(colour_to_symbol_map->root, colour_to_floss_map, floss_to_symbol_map);
	return floss_to_symbol_map;
}


struct rb_tree* count_floss_stitches(struct rb_tree* colour_to_floss_map, uint* px, uint width, uint height)
{
	struct rb_tree* floss_to_stitch_count_map = create_rb_tree();

	for(uint i = 0; i < width * height; i++)
	{
		uint* src_px_index = px + 3*i;
		uint colour = hash_RGB(*src_px_index, *(src_px_index + 1), *(src_px_index + 2));
		uint floss_code = (*colour_to_floss_map)[colour];
		(*floss_to_stitch_count_map)[floss_code]++;
	}
	
	return floss_to_stitch_count_map;
}

void map_colour_to_symbol(struct node* n, uint& symbol_index)
{
	if(n)
	{
		map_colour_to_symbol(n->left_child, symbol_index);

		struct kv_pair* p = &(n->pair);
		if(p->key == 0xffffff) p->value = 0;
		else if(p->key == 0) p->value = 1;
		else 
		{
			p->value = symbol_index;
			symbol_index++;
		
		}

		map_colour_to_symbol(n->right_child, symbol_index);
	}
}

struct rb_tree* map_src_colours_to_symbols(uint* src_colours, uint src_width, uint src_height)
{
	struct rb_tree* colour_map = create_rb_tree();
	for(uint i = 0; i < src_width * src_height; i++)
	{
		uint* px_RGB = src_colours + 3*i;
		uint key = hash_RGB(*px_RGB, *(px_RGB + 1), *(px_RGB + 2));
		(*colour_map)[key];
	}
	uint symbol_index = 2; // Makes sure white and black will be set to specific symbols
	map_colour_to_symbol(colour_map->root, symbol_index);
	return colour_map;
}

uint** load_symbols()
{
	 uint** symbols = new uint*[NUM_SYMBOLS];
	
	for(uint i = 0; i < NUM_SYMBOLS; i++)
	{
		char* path = new char[strlen(symbol_prefix) + strlen(ppm_extension) + 2];
		sprintf(path, "%s%02d%s", symbol_prefix, i, ppm_extension);
		struct ppm_file_data* f = parse_ppm_file(path);
		symbols[i] = f->colour_vals;
		delete[] path;
		destroy_file(f, 0);
	}
	return symbols;
}

void destroy_symbols(uint** symbols)
{
	for(uint i = 0; i < NUM_SYMBOLS; i++) delete[] symbols[i];
	delete[] symbols;
}


void copy_px_RGB(uint* src_px, uint* dest_px)
{
	for(uint i = 0; i < 3; i++) *(dest_px + i) = *(src_px + i);
}

//Transform src ppm => {colour_map} =>  pattern_image ppm
void populate_pattern_colour_data(uint* src_colours, uint src_width, uint src_height, uint* pattern_colours, uint pattern_width, uint pattern_height, struct rb_tree* colour_to_symbol_map)
{
	uint** ps_list = load_symbols();
	for(uint pat_px_index = 0; pat_px_index < pattern_width * pattern_height; pat_px_index++)
	{
		uint pat_px_row = (uint)(pat_px_index / pattern_width);
		uint pat_px_col = (uint)(pat_px_index % pattern_width);

		//Find current px's corresponding px in the source image
		uint src_px_row = pat_px_row / 16;
		uint src_px_col = pat_px_col / 16;
		
		uint symbol_px_row = pat_px_row % 16;
		uint symbol_px_col = pat_px_col % 16;

		uint src_px_index = src_px_row*src_width + src_px_col;
		uint symbol_px_index = 3*(16*symbol_px_row + symbol_px_col);
		
		uint* src_px_RGB = src_colours + 3*src_px_index;
		
		uint key = hash_RGB(*src_px_RGB, *(src_px_RGB + 1), *(src_px_RGB + 2));
		uint* symbol = ps_list[(*colour_to_symbol_map)[key]];

		//Source pxs
		uint* symbol_px_RGB = symbol + symbol_px_index;

		uint* dest_px_RGB = pattern_colours + 3*pat_px_index;
		uint symbol_px_colour = hash_RGB(*symbol_px_RGB, *(symbol_px_RGB + 1), *(symbol_px_RGB + 2));
		uint* result_px_RGB = (symbol_px_colour == 0xffffff) ? src_px_RGB : symbol_px_RGB;
		copy_px_RGB(result_px_RGB, dest_px_RGB);
	}
	destroy_symbols(ps_list);
}

void create_pattern_image(uint* src_colours, uint src_width, uint src_height, uint page, struct rb_tree* colour_to_symbol_map)
{
	uint pattern_width = 	src_width * 16;
	uint pattern_height = 	src_height * 16;

	uint* pattern_image_px = new uint[3*pattern_width * pattern_height];
	
	populate_pattern_colour_data(src_colours, src_width, src_height, pattern_image_px, pattern_width, pattern_height, colour_to_symbol_map);

	struct ppm_file_data* final_image = create_ppm_data(pattern_width, pattern_height);
	final_image->colour_vals = pattern_image_px;
	
	char* path = new char[14];
	sprintf(path, "output_%02d.ppm", page);
	print_to_ppm(path, final_image);
	
	delete[] path;
	destroy_file(final_image);
}

void create_pattern_info(struct ppm_file_data* src_image, struct rb_tree* colour_to_floss_map)
{
	struct rb_tree* floss_to_stitch_count_map = count_floss_stitches(colour_to_floss_map, src_image->colour_vals, src_image->width, src_image->height);
	
	struct pattern_info* p_info = new struct pattern_info;
	
	p_info->width_in_stitches = 		src_image->width;
	p_info->height_in_stitches = 		src_image->height;
	p_info->number_of_stitches = 		p_info->width_in_stitches * p_info->height_in_stitches;
	p_info->floss_to_stitch_count_map = 	floss_to_stitch_count_map;
	
	print_pattern_info(p_info);

	destroy_rb_tree(p_info->floss_to_stitch_count_map);
	delete p_info;
}

uint** split_src_into_pages(struct ppm_file_data* src_image, uint page_stitch_length, uint& page_count)
{
	uint page_width = (uint) ceil((float)src_image->width / (float)page_stitch_length);
	uint page_height = (uint) ceil((float)src_image->height / (float)page_stitch_length);
	page_count = page_width * page_height;

	uint** pages = new uint*[page_count];
	
	for(uint i = 0; i < page_count; i++)
	{
		uint* page = new uint[3*page_stitch_length * page_stitch_length];
		for(uint j = 0; j < page_stitch_length*page_stitch_length; j++)
		{
			// Get src px
			uint white[3] = {0xff, 0xff, 0xff};

			uint src_px_row = (i/page_width)*page_stitch_length + (j/page_stitch_length);
			uint src_px_col = (i%page_width)*page_stitch_length + (j % page_stitch_length);
			uint src_px_index = src_px_row*src_image->width + src_px_col;

			uint* src_px = (src_px_row >= src_image->height || src_px_col >= src_image->width) ? white : src_image->colour_vals + 3 * src_px_index;
			
			uint* dest_px = page + 3*j;
			// Copy src to dest
			copy_px_RGB(src_px, dest_px);
		}
		pages[i] = page;
	}

	return pages;	
}

void destroy_pages(uint** src_pages, uint page_count)
{
	for(uint i = 0; i < page_count; i++) delete[] src_pages[i];
	delete[] src_pages;
}

void create_pattern_from_src(struct ppm_file_data* src_image)
{
	struct rb_tree* colour_to_symbol_map = 		map_src_colours_to_symbols(src_image->colour_vals, src_image->width, src_image->height);	
	struct rb_tree* colour_to_floss_map = 		create_DMC_floss_map();

	uint page_stitch_length = 65; // Temporary for PoC
	uint page_count;
	
	uint** src_pages = split_src_into_pages(src_image, page_stitch_length, page_count);

	printf("Required pages = %d\n", page_count);

	for(uint i = 0; i < page_count; i++)
	{
		create_pattern_image(src_pages[i], page_stitch_length, page_stitch_length, i, colour_to_symbol_map);
	}

	create_pattern_info(src_image, colour_to_floss_map);
	
	struct rb_tree* floss_to_symbol_map = create_floss_to_symbol_map(colour_to_symbol_map, colour_to_floss_map); //TODO: Print this info to file


	destroy_pages(src_pages, page_count);

	destroy_rb_tree(floss_to_symbol_map);
	destroy_rb_tree(colour_to_floss_map);
	destroy_rb_tree(colour_to_symbol_map);
}


byte create_pattern(const char* src_image_path)
{	
	byte pattern_created = 0;
	struct ppm_file_data* src_image = parse_ppm_file(src_image_path);
	if(src_image)
	{
		create_pattern_from_src(src_image);
		pattern_created = 1;		
	}

	destroy_file(src_image);
	return pattern_created;
}
