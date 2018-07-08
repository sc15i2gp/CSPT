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

void destroy_glyphs(uint** glyphs)
{
	for(uint i = 0; i < NUM_GLYPHS; i++)
	{
		delete[] glyphs[i];
	}
	delete[] glyphs;
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

void print_floss_stitch_count_pair(struct kv_pair* p, uint colour)
{
	printf("Floss %d has %d stitches\n", p->key, p->value);
}

void print_floss_to_stitch_count_map(struct rb_tree* t)
{
	printf("===========================================\n");
	execute_for_each_pair(t, print_floss_stitch_count_pair);
	printf("===========================================\n\n");
}

void sum_stitch_value(struct kv_pair* p, uint* count)
{
	*count += p->value;
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

uint count_colours_in_file(struct ppm_file_data* file_data)
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
	
	uint count = 0;
	execute_for_each_pair(colour_map, &count, sum_stitch_value);
	destroy_rb_tree(colour_map);
	
	return count;
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

void copy_px_RGB(uint* src_px, uint* dest_px)
{
	for(uint i = 0; i < 3; i++) *(dest_px + i) = *(src_px + i);
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
		copy_px_RGB(result_px_RGB, pat_px_RGB);
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
				if(i - first_code_cell < code_str_length)
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
uint* create_floss_to_symbol_pattern_map(struct rb_tree* floss_to_symbol_map, uint* px_map_width, uint* px_map_height)
{
	uint floss_count = count_nodes(floss_to_symbol_map);
	uint pattern_map_height = 1 /*top padding*/ + 2 /*title + padding*/ + (2 * floss_count) /*each floss + padding*/;
	uint pattern_map_width = 2 /*left/right padding*/ + 3 /*symbol + right padding*/ + 4 /*floss code*/;

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
	uint** symbols = load_symbols();
	uint** glyphs = load_glyphs();
	
	uint cell_size_coefficient = 2;
	uint px_cell_length = cell_size_coefficient * 16;

	uint output_px_count = pattern_map_height * pattern_map_width * (px_cell_length*px_cell_length);

	uint* floss_to_symbol_pattern = new uint[3*output_px_count];

	for(uint i = 0; i < output_px_count; i++)
	{
		uint output_px_row = (uint)(i / (pattern_map_width*px_cell_length));
		uint output_px_col = (uint)(i % (pattern_map_width*px_cell_length));
		uint cell_row = (uint)(output_px_row / px_cell_length);
		uint cell_col = (uint)(output_px_col / px_cell_length);
		uint cell_index = (cell_row*pattern_map_width) + cell_col;

		
		uint first_px_in_cell_row = px_cell_length * cell_row;
		uint first_px_in_cell_col = px_cell_length * cell_col;

		uint src_px_row = output_px_row - first_px_in_cell_row;
		uint src_px_col = output_px_col - first_px_in_cell_col;
		uint src_px_cell_length = 16; //(uint)(px_cell_length / cell_size_coefficient);
		uint src_index = ((src_px_row/cell_size_coefficient)*src_px_cell_length) + (src_px_col / cell_size_coefficient);
		uint* src_px;

		enum cell_contents_type type = pattern_map_cells[cell_index].type;
		if(type == CELL_SYMBOL)
		{
			src_px = symbols[pattern_map_cells[cell_index].ref] + (3*src_index);
		}
		else if(type == CELL_GLYPH)
		{
			src_px = glyphs[pattern_map_cells[cell_index].ref] + (3*src_index);
		}
		else
		{
			uint px[3] = {0xff, 0xff, 0xff};
			src_px = px;
		}

		uint* dest_px = floss_to_symbol_pattern + 3*i;

		copy_px_RGB(src_px, dest_px);
	}

	struct ppm_file_data* file = new struct ppm_file_data;
	file->type = 3;
	file->width = pattern_map_width*px_cell_length;
	file->height = pattern_map_height*px_cell_length;
	file->max_val = 255;
	file->colour_vals = floss_to_symbol_pattern;

	print_to_ppm("output_map.ppm", file);
	destroy_symbols(symbols);
	destroy_glyphs(glyphs);

	delete[] pattern_map_cells;
	*px_map_width = pattern_map_width * px_cell_length;
	*px_map_height = pattern_map_height * px_cell_length;
	return floss_to_symbol_pattern;
}



struct pattern_info* create_pattern_from_src(struct ppm_file_data* src_image)
{
	
	//Map of hashed RGB => pattern symbol
	struct rb_tree* colour_to_symbol_map = map_src_RGB_to_symbols(src_image->colour_vals, src_image->width, src_image->height);
	
	struct rb_tree* colour_to_floss_map = create_DMC_floss_map();
	struct rb_tree* floss_to_symbol_map = create_floss_to_symbol_map(colour_to_symbol_map, colour_to_floss_map);

	uint pattern_map_width;
	uint pattern_map_height;
	uint* pattern_map_px = create_floss_to_symbol_pattern_map(floss_to_symbol_map, &pattern_map_width, &pattern_map_height);

	

	uint pattern_width = src_image->width * 16;
	uint pattern_height = src_image->height * 16;
	uint* pattern_image_px = new uint[3*pattern_width * pattern_height];
	
	struct rb_tree* floss_to_stitch_count_map = create_rb_tree();

	struct pattern_info* p_info = new struct pattern_info;
	p_info->width_in_stitches = src_image->width;
	p_info->height_in_stitches = src_image->height;
	p_info->number_of_stitches = p_info->width_in_stitches * p_info->height_in_stitches;
	for(uint i = 0; i < src_image->width * src_image->height; i++)
	{
		uint* src_px_index = src_image->colour_vals + 3*i;
		uint colour = hash_RGB(*src_px_index, *(src_px_index + 1), *(src_px_index + 2));
		uint floss_code = (*colour_to_floss_map)[colour];
		(*floss_to_stitch_count_map)[floss_code]++;
	}

	p_info->floss_to_stitch_count_map = floss_to_stitch_count_map;

	populate_pattern_colour_data(src_image->colour_vals, src_image->width, src_image->height, pattern_image_px, pattern_width, pattern_height, colour_to_symbol_map);


	uint final_width = pattern_width + pattern_map_width;
	uint final_height = (pattern_height > pattern_map_height) ? pattern_height : pattern_map_height;
	uint final_px_count = final_width * final_height;
	uint* final_image_px = new uint[3 * final_px_count];

	uint white_px[3] = {0xff, 0xff, 0xff};

	printf("pattern width + pattern map width = %d + %d = %d\n", pattern_width, pattern_map_width, final_width);
	printf("pattern height + pattern map height = %d + %d = %d\n", pattern_height, pattern_map_height, final_height);
	
	for(uint i = 0; i < final_px_count; i++)
	{
		uint final_px_row = (uint)(i / final_width);
		uint final_px_col = (uint)(i % final_width);

		uint src_row = final_px_row;
		uint src_col = final_px_col;
		uint* src_px;
		if(final_px_col < pattern_width)
		{
			// If should copy px from pattern_image_px
			uint src_index = src_row*pattern_width + src_col;
			if(src_index < pattern_width * pattern_height)
			{
				src_px = pattern_image_px + 3*src_index;
			}
			else
			{
				src_px = white_px;
			}
		}
		else
		{
			// If should copy px from pattern_map_px
			src_col -= pattern_width;
			uint src_index = src_row*pattern_map_width + src_col;
			if(src_index < pattern_map_width * pattern_map_height)
			{
				src_px = pattern_map_px + 3*src_index;
			}
			else
			{
				src_px = white_px;
			}
		}
		uint final_px_index = final_px_row*final_width + final_px_col;
		uint* dest_px = final_image_px + 3*final_px_index;
		copy_px_RGB(src_px, dest_px);
	}
	
	delete[] pattern_map_px;
	delete[] pattern_image_px;

	struct ppm_file_data* final_image = new struct ppm_file_data;
	final_image->type = 3;
	final_image->width = final_width;
	final_image->height = final_height;
	final_image->max_val = 255;
	final_image->colour_vals = final_image_px;
	
	print_to_ppm("output.ppm", final_image);

	
	destroy_rb_tree(floss_to_symbol_map);
	destroy_rb_tree(colour_to_floss_map);
	destroy_file(final_image);
	destroy_rb_tree(colour_to_symbol_map);
	printf("Done creating pattern\n");
	return p_info;
}

byte create_pattern(const char* src_image_path)
{	
	byte pattern_created = 0;
	struct ppm_file_data* src_image = parse_ppm_file(src_image_path);
	if(src_image)
	{
		struct pattern_info* p_info = create_pattern_from_src(src_image);
		pattern_created = 1;
		
		printf("\n================================================\n");
		printf("Pattern output:\n");
		printf("Width = %d stitches\n", p_info->width_in_stitches);
		printf("Height = %d stitches\n", p_info->height_in_stitches);
		printf("Total = %d stitches\n", p_info->number_of_stitches);
		printf("Number of flosses = %d\n", p_info->number_of_colours);
		print_floss_to_stitch_count_map(p_info->floss_to_stitch_count_map);	
		printf("\n================================================\n");

		destroy_rb_tree(p_info->floss_to_stitch_count_map);
		delete p_info;	
	}

	destroy_file(src_image);
	return pattern_created;
}
