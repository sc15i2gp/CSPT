#include "Pattern.h"

struct rb_tree* create_DMC_floss_map()
{
	struct rb_tree* floss_map = create_rb_tree();
	
	for(uint i = 0; i < FLOSS_COUNT; i++)
	{
		(*floss_map)[DMC_flosses[i].key] = DMC_flosses[i].value;
	}

	return floss_map;
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
	for(uint i = 0; i < NUM_SYMBOLS; i++)
	{
		delete[] symbols[i];
	}
	delete[] symbols;
}

uint** load_glyphs()
{
	printf("Loading glyphs...\n");

	uint** glyphs = new uint*[NUM_GLYPHS];

	for(uint i = 0; i < NUM_GLYPHS; i++)
	{
		char glyph[2];
		switch(i)
		{
			case GLYPH_D:
				glyph[0] = 'D';
				break;
			case GLYPH_M:
				glyph[0] = 'M';
				break;
			case GLYPH_C:
				glyph[0] = 'C';
				break;
			default:
				sprintf(glyph, "%d", i - GLYPH_0);
				break;
		}
		glyph[1] = 0;
		char* path = new char[strlen(glyph_prefix) + strlen(ppm_extension) + 1];
		printf("%d path: %s%s%s\n", i, glyph_prefix, glyph, ppm_extension);
		sprintf(path, "%s%s%s", glyph_prefix, glyph, ppm_extension);
		struct ppm_file_data* f = parse_ppm_file(path);
		glyphs[i] = f->colour_vals;
		delete[] path;
		destroy_file(f, 0);
	}

	printf("Glyphs loaded\n");
	return glyphs;
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

struct rb_tree* load_file_colours_into_map(struct ppm_file_data* file_data)
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

void count_colours_in_file(struct ppm_file_data* file_data)
{
	printf("Counting...\n");
	struct rb_tree* colour_map = load_file_colours_into_map(file_data);
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

struct rb_tree* map_src_RGB_to_symbols(uint* src_colours, uint src_width, uint src_height)
{
	struct rb_tree* colour_map = create_rb_tree();
	uint i_max = src_width * src_height;
	uint symbol_index = 1;
	for(uint i = 0; i < i_max; i++)
	{
		uint* px_RGB = src_colours + 3*i;
		uint key = hash_RGB(*px_RGB, *(px_RGB + 1), *(px_RGB + 2));
		//White should always map to the first symbol, black second
		if(!is_key_in_tree(colour_map, key))
		{
			if(key == 0xffffff) (*colour_map)[key] = 0;
			else if(key == 0) (*colour_map)[key] = 1;
			else
			{
				(*colour_map)[key] = symbol_index;
				symbol_index++;
			}
		}
	}
	return colour_map;
}

struct ppm_file_data* create_pattern_image_info(uint src_width, uint src_height)
{
	struct ppm_file_data* pattern_image = new struct ppm_file_data;
	uint type = 3;
	//One pixel in src corresponds to a 16*16 symbol in output file
	uint width = src_width * 16;
	uint height = src_height * 16;
	uint max_val = 256;

	pattern_image->type = type; //Set 3 as the default
	pattern_image->width = width;
	pattern_image->height = height;
	pattern_image->max_val = max_val;

	allocate_colour_data(pattern_image);
	return pattern_image;	
}

void populate_pattern_colour_data(uint* src_colours, uint src_width, uint src_height, uint* pattern_colours, uint pattern_width, uint pattern_height, struct rb_tree* colour_map)
{
	uint** ps_list = load_symbols();
	//Transform src => {colour_map} =>  pattern_image
	//For each pixel in pattern_image
	for(uint pat_px_index = 0; pat_px_index < pattern_width * pattern_height; pat_px_index++)
	{
		uint pat_px_row = (uint)(pat_px_index / pattern_width);
		uint pat_px_col = (uint)(pat_px_index % pattern_width);

		//Find current pixel's RBG in the source image
		uint src_px_row = (uint) (pat_px_row / 16);
		uint src_px_col = (uint) (pat_px_col / 16);
		uint src_px_index = 3*(src_px_row*src_width + src_px_col);
		
		uint* src_px_RGB = src_colours + src_px_index;
		uint key = hash_RGB(*src_px_RGB, *(src_px_RGB + 1), *(src_px_RGB + 2));
		uint* symbol = ps_list[(*colour_map)[key]];

		//Get symbol's pixel RGB to put in pat_px_index
		uint symbol_px_row = pat_px_row % 16;
		uint symbol_px_col = pat_px_col % 16;

		uint symbol_px_index = 3*(16*symbol_px_row + symbol_px_col);

		uint* symbol_px_RGB = symbol + symbol_px_index;
		uint symbol_px_colour = hash_RGB(*symbol_px_RGB, *(symbol_px_RGB + 1), *(symbol_px_RGB + 2));
		uint* pat_px_RGB = pattern_colours + 3*pat_px_index;
		uint* result_px_RGB = (symbol_px_colour == 0xffffff) ? src_px_RGB : symbol_px_RGB;
		
		*pat_px_RGB = 		*result_px_RGB;
		*(pat_px_RGB + 1) = 	*(result_px_RGB + 1);
		*(pat_px_RGB + 2) = 	*(result_px_RGB + 2);
	}
	destroy_symbols(ps_list);
}

void set_floss_to_symbol_pair(uint colour, uint symbol, struct rb_tree* floss_to_symbol_map, struct rb_tree* colour_to_floss_map)
{
	(*floss_to_symbol_map)[(*colour_to_floss_map)[colour]] = symbol;
}

void set_floss_to_symbol_pair(struct node* n, struct rb_tree* colour_to_floss_map, struct rb_tree* floss_to_symbol_map)
{
	if(n)
	{
		struct kv_pair p = n->pair;
		set_floss_to_symbol_pair(n->left_child, colour_to_floss_map, floss_to_symbol_map);
		set_floss_to_symbol_pair(p.key, p.value, floss_to_symbol_map, colour_to_floss_map);
		set_floss_to_symbol_pair(n->right_child, colour_to_floss_map, floss_to_symbol_map);
	}	
}

void set_floss_to_symbol_pairs(struct rb_tree* colour_to_symbol_map, struct rb_tree* colour_to_floss_map, struct rb_tree* floss_to_symbol_map)
{
	set_floss_to_symbol_pair(colour_to_symbol_map->root, colour_to_floss_map, floss_to_symbol_map);
}

struct rb_tree* create_floss_to_symbol_map(struct rb_tree* colour_to_symbol_map, struct rb_tree* colour_to_floss_map)
{
	struct rb_tree* floss_to_symbol_map = create_rb_tree();

	// For each colour
	// 	Set symbol_to_floss[colour_to_floss[colour]] = colour_to_symbol[colour]
	set_floss_to_symbol_pairs(colour_to_symbol_map, colour_to_floss_map, floss_to_symbol_map);
	return floss_to_symbol_map;
}


enum cell_contents_type
{
	CELL_EMPTY,
	CELL_SYMBOL,
	CELL_GLYPH
};

struct cell_contents
{
	enum cell_contents_type type;
	uint ref;	
};

void set_padding_row(struct cell_contents* pattern_map_cells, uint row_width, uint& current_cell)
{
	for(uint i = 0; i < row_width; i++, current_cell++)
	{
		pattern_map_cells[current_cell].type = CELL_EMPTY;
	}
}

void set_title_row(struct cell_contents* pattern_map_cells, uint row_width, uint& current_cell)
{
	uint cell_D = 1;
	uint cell_M = 2;
	uint cell_C = 3;

	for(uint i = 0; i < row_width; i++, current_cell++)
	{
		enum cell_contents_type type;
		uint ref = 0;
		if(i == cell_D)
		{
			ref = GLYPH_D;
			type = CELL_GLYPH;
		}
		else if(i == cell_M)
		{
			ref = GLYPH_M;
			type = CELL_GLYPH;
		}
		else if(i == cell_C)
		{
			ref = GLYPH_C;
			type = CELL_GLYPH;
		}
		else
		{
			type = CELL_EMPTY;
		}
		pattern_map_cells[current_cell].type = type;
		pattern_map_cells[current_cell].ref = ref;
	}
	
}

void set_floss_to_symbol_row(struct node* n, struct cell_contents* pattern_map_cells, uint row_width, uint& current_cell)
{
	if(n)
	{
		uint floss_code = n->pair.key;
		uint symbol = n->pair.value;
		char code_str[5];
		sprintf(code_str, "%d", floss_code);
		uint code_str_length = strlen(code_str);
		
		set_floss_to_symbol_row(n->left_child, pattern_map_cells, row_width, current_cell);

		uint symbol_cell = 1;
		uint first_code_cell = 4;
		uint last_code_cell = 7;

		for(uint i = 0; i < row_width; i++, current_cell++)
		{
			if(i == symbol_cell)
			{
				pattern_map_cells[current_cell].type = CELL_SYMBOL;
				pattern_map_cells[current_cell].ref = symbol;
			}
			else if(i >= first_code_cell && i <= last_code_cell)
			{
				if(i - first_code_cell <= code_str_length)
				{
					char code_digit_str[2] = {code_str[i - first_code_cell], 0};

					uint code_digit = atoi(code_digit_str);
					pattern_map_cells[current_cell].type = CELL_GLYPH;
					pattern_map_cells[current_cell].ref = GLYPH_0 + code_digit;
				}
				else pattern_map_cells[current_cell].type = CELL_EMPTY;
			}
			else pattern_map_cells[current_cell].type = CELL_EMPTY;
		}

		set_padding_row(pattern_map_cells, row_width, current_cell);
		
		set_floss_to_symbol_row(n->right_child, pattern_map_cells, row_width, current_cell);
	}
}

void set_floss_to_symbol_rows(struct rb_tree* floss_to_symbol_map, struct cell_contents* pattern_map_cells, uint row_width, uint& current_cell)
{
	set_floss_to_symbol_row(floss_to_symbol_map->root, pattern_map_cells, row_width, current_cell);	
}

// Generated the graphical version of a floss => symbol map
void create_floss_to_symbol_pattern_map(struct rb_tree* floss_to_symbol_map)
{
	uint cell_size_coefficient = 1;
	uint px_cell_length = cell_size_coefficient * 16;

	uint** symbols = load_symbols();
	uint** glyphs = load_glyphs();
	
	uint floss_count = count_nodes(floss_to_symbol_map);
	uint pattern_map_height = 1 /*top padding*/ + 2 /*title + padding*/ + (2 * floss_count) /*each floss + padding*/;
	uint pattern_map_width = 2 /*left/right padding*/ + 3 /*symbol + right padding*/ + 5 /*floss code + right padding*/;

	struct cell_contents* pattern_map_cells = new struct cell_contents[pattern_map_height*pattern_map_width];

	uint current_cell = 0;

	//Title + padding
	printf("Setting title + padding\n");
	set_padding_row(pattern_map_cells, pattern_map_width, current_cell);
	set_title_row(pattern_map_cells, pattern_map_width, current_cell);
	set_padding_row(pattern_map_cells, pattern_map_width, current_cell);

	//Actual map + padding
	printf("Setting floss to symbol map\n");
	set_floss_to_symbol_rows(floss_to_symbol_map, pattern_map_cells, pattern_map_width, current_cell);
	printf("Done making pattern map\n");
	
	//Convert pattern_map_cells to uint* colour vals

	delete[] pattern_map_cells;
}



void create_pattern_from_src(struct ppm_file_data* src_image)
{
	count_colours_in_file(src_image);
	
	
	
	//Map of hashed RGB => pattern symbol
	struct rb_tree* colour_to_symbol_map = map_src_RGB_to_symbols(src_image->colour_vals, src_image->width, src_image->height);
	
	struct rb_tree* colour_to_floss_map = create_DMC_floss_map();
	struct rb_tree* floss_to_symbol_map = create_floss_to_symbol_map(colour_to_symbol_map, colour_to_floss_map);
	
	create_floss_to_symbol_pattern_map(floss_to_symbol_map);

	
	
	struct ppm_file_data* pattern_image = create_pattern_image_info(src_image->width, src_image->height);
	
	populate_pattern_colour_data(src_image->colour_vals, src_image->width, src_image->height, pattern_image->colour_vals, pattern_image->width, pattern_image->height, colour_to_symbol_map);

	
	
	print_to_ppm("output.ppm", pattern_image);

	
	destroy_rb_tree(floss_to_symbol_map);
	destroy_rb_tree(colour_to_floss_map);
	destroy_file(pattern_image);
	destroy_rb_tree(colour_to_symbol_map);
	printf("Done creating pattern\n");
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
