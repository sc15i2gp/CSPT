#include "ParseState.h"

byte is_digit(const char c)
{
	for(uint i = 0; i < 10; i++)
	{
		if(digits[i] == c) return 1;
	}
	return 0;
}

byte is_whitespace(const char c)
{
	for(uint i = 0; i < 3; i++)
	{
		if(whitespace[i] == c) return 1;
	}
	return 0;
}


void clear_state_char_buffer(struct parse_state_data* data)
{
	for(uint i = 0; i < 8; i++) data->buffer[i] = 0;
}


void set_state_buffer_char(struct parse_state_data* state_info, const char c)
{
	state_info->buffer[state_info->char_buffer_pos] = c;
	state_info->char_buffer_pos++;
}

enum state_return_code start_state(const char input, struct parse_state_data* state_info)
{
	if(input == 'P') return RC_TRANSITION;
	else return RC_INVALID;
}

enum state_return_code handle_char(const char input, struct parse_state_data* state_info)
{
		if(is_digit(input)) 
		{
			set_state_buffer_char(state_info, input);
			return RC_REPEAT;
		}
		else if(is_whitespace(input)) 
		{
			*(state_info->file_info_field.uint_field) = (uint)atoi(state_info->buffer);
			return RC_TRANSITION;
		}
		else return RC_INVALID;
}

enum state_return_code handle_first_char(const char input, struct parse_state_data* state_info)
{
	if(is_digit(input))
	{
		state_info->is_whitespace_valid = 1;
		set_state_buffer_char(state_info, input);
		return RC_REPEAT;
	}
	else return RC_INVALID;	
}

enum state_return_code type_state(const char input, struct parse_state_data* state_info)
{
	if(!state_info->is_whitespace_valid)
	{
		return handle_first_char(input, state_info);
	}
	else
	{
		enum state_return_code rc = handle_char(input, state_info);

		return (rc != RC_REPEAT) ? rc : RC_INVALID;
	}
	return RC_INVALID;
}


enum state_return_code info_number_state(const char input, struct parse_state_data* state_info)
{
	if(!state_info->is_whitespace_valid)
	{
		return handle_first_char(input, state_info);
	}
	else
	{
		return handle_char(input, state_info);
	}
	return RC_INVALID;
}

enum state_return_code colour_number_state(const char input, struct parse_state_data* state_info)
{
	if(!state_info->is_whitespace_valid)
	{
		if(input == 0) return RC_END;
		else return handle_first_char(input, state_info);
	}
	else
	{
		enum state_return_code rc = handle_char(input, state_info);
		if(rc == RC_TRANSITION) state_info->data_field_pos++;
		return rc;
	}
	return RC_INVALID;
}

StateFunction lookup_state_function(enum parse_state state)
{
	assert(state != P_STATE_INVALID);
	switch(state)
	{
		case P_STATE_START:
			return state_functions[0];
		case P_STATE_TYPE:
			return state_functions[1];
		case P_STATE_WIDTH:
		case P_STATE_HEIGHT:
		case P_STATE_MAX:
			return state_functions[2];
		default:
			return state_functions[3];
	}
}

enum parse_state transition_state(enum parse_state p_state, enum state_return_code rc)
{
	enum parse_state dest_state = P_STATE_INVALID;
	for(uint i = 0; i < 9; i++)
	{
		const struct transition* t = transitions + i;
		if(p_state == t->src_state && rc == t->rc) dest_state = t->dest_state;
	}
	assert(dest_state != P_STATE_INVALID);
	return dest_state;
}

void reset_parse_state_data(struct parse_state_data* data)
{
	data->is_whitespace_valid = 0;
	clear_state_char_buffer(data);
	data->char_buffer_pos = 0;
	data->file_info_field.byte_field = NULL;
}

void prepare_state(enum parse_state p_state, struct parse_state_data* state_data, StateFunction* state_func)
{
	*state_func = lookup_state_function(p_state);
	reset_parse_state_data(state_data);
}
