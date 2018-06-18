#ifndef PARSE_STATE_H
#define PARSE_STATE_H

#include <stdlib.h>
#include <assert.h>

typedef unsigned char byte;
typedef unsigned int uint;


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

void prepare_state(enum parse_state p_state, struct parse_state_data* state_data, StateFunction* state_func);

void reset_parse_state_data(struct parse_state_data* data);

enum parse_state transition_state(enum parse_state p_state, enum state_return_code rc);

#endif

