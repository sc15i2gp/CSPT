#ifndef PATTERN_H
#define PATTERN_H

#include "Map.h"
#include "FileInfo.h"
#include "Parse.h"

static const char* symbol_prefix = "Symbols/S_";
static const char* symbol_extension = ".ppm";

#define NUM_SYMBOLS 31

byte create_pattern(const char*);

#endif
