#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include "FileInfo.h"
#include "ParseState.h"

typedef unsigned char byte;
typedef unsigned int uint;


struct file_info* process_file(const char* file_path);

void reset_parse_state_data(struct parse_state_data* data);

byte parsePPM(const char* ppm, struct file_info* file_data);

#endif
