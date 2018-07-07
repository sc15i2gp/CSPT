#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include "ParseState.h"
#include "PPMFileData.h"

typedef unsigned char byte;
typedef unsigned int uint;


struct ppm_file_data* parse_ppm_file(const char* file_path);

void reset_parse_state_data(struct parse_state_data* data);

byte parsePPM(const char* ppm, struct ppm_file_data* file_data);

#endif
