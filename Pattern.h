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

byte create_pattern(const char*);

#endif
