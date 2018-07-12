#ifndef PATTERN_H
#define PATTERN_H

#include <math.h>
#include "Map.h"
#include "PPMFileData.h"
#include "Parse.h"
#include "DMC.h"

static const char* symbol_prefix = "Symbols/S_";
static const char* glyph_prefix = "Symbols/G_";
static const char* ppm_extension = ".ppm";

#define NUM_SYMBOLS 31
#define NUM_GLYPHS 13

#define GLYPH_D 0
#define GLYPH_M 1
#define GLYPH_C 2
#define GLYPH_0 3

struct pattern_info
{
	uint width_in_stitches;
	uint height_in_stitches;
	uint number_of_stitches;
	uint number_of_colours;
	uint number_of_pages;
	uint length_of_page; // In stitches
	struct rb_tree* floss_to_stitch_count_map;
};

byte create_pattern(const char* src_path, const char* output_dir, uint page_stitch_length);

#endif
