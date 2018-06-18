#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

typedef unsigned char byte;
typedef unsigned int uint;

struct file_info
{
	byte 	type;
	uint 	width;
	uint 	height;
	uint 	max_val;
	uint* 	colour_vals;
};

struct parse_state_data
{
	byte is_whitespace_valid;
	char buffer[8];
	uint char_buffer_pos;
	uint data_field_pos;
	union
	{
		byte* byte_field;
		uint* uint_field;
	} file_info_field;
};

void allocate_colour_data(struct file_info* file_data);

struct file_info* process_file(const char* file_path);

void destroy_file(struct file_info* file_data);

void print_to_ppm(const char* file_name, struct file_info* file_data);

void reset_parse_state_data(struct parse_state_data* data);

static const char digits[10] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

static const char whitespace[3] =
{
	' ', '\n', '\t'
};

enum parse_state
{
	P_STATE_INVALID = -1,
	P_STATE_START = 0,
	P_STATE_TYPE,
	P_STATE_WIDTH,
	P_STATE_HEIGHT,
	P_STATE_MAX,
	P_STATE_R,
	P_STATE_G,
	P_STATE_B,
	P_STATE_END
};

enum state_return_code
{
	RC_INVALID = -1,
	RC_TRANSITION,
	RC_REPEAT,
	RC_END
};

struct transition
{
	enum parse_state src_state;
	enum state_return_code rc;
	enum parse_state dest_state;
};

typedef enum state_return_code (*StateFunction)(const char input, struct parse_state_data* state_info);

enum state_return_code start_state(const char input, struct parse_state_data* state_info);

enum state_return_code type_state(const char input, struct parse_state_data* state_info);

enum state_return_code info_number_state(const char input, struct parse_state_data* state_info);

enum state_return_code colour_number_state(const char input, struct parse_state_data* state_info);

static const StateFunction state_functions[] =
{
	start_state, type_state, info_number_state, colour_number_state
};

static const struct transition transitions[9] = 
{
//	Source state		Transition symbol	Destination state
	{P_STATE_START, 	RC_TRANSITION, 		P_STATE_TYPE},
	{P_STATE_TYPE, 		RC_TRANSITION, 		P_STATE_WIDTH},
	{P_STATE_WIDTH, 	RC_TRANSITION, 		P_STATE_HEIGHT},
	{P_STATE_HEIGHT, 	RC_TRANSITION, 		P_STATE_MAX},
	{P_STATE_MAX, 		RC_TRANSITION, 		P_STATE_R},
	{P_STATE_R, 		RC_TRANSITION, 		P_STATE_G},
	{P_STATE_G, 		RC_TRANSITION, 		P_STATE_B},
	{P_STATE_B, 		RC_TRANSITION, 		P_STATE_R},
	{P_STATE_B, 		RC_END, 		P_STATE_END}
};

byte parsePPM(const char* ppm, struct file_info* file_data);

#endif
