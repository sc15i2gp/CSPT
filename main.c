#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

//I want to specify a file name and the program should read it:
//Outputting its width and height
//	Open file with given file name

static const char digits[10] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

static const char whitespace[3] =
{
	' ', '\n', '\t'
};

typedef unsigned char byte;
typedef unsigned int uint;

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

struct file_info
{
	byte 	type;
	uint 	width;
	uint 	height;
	uint 	max_val;
	uint* 	colour_vals;
};

void allocate_colour_data(struct file_info* file_data)
{
	uint number_of_colour_vals = file_data->width * file_data->height;
	printf("Allocating %d pixles of uints\n", number_of_colour_vals);
	file_data->colour_vals = malloc(3 * number_of_colour_vals * sizeof(uint));
}

typedef enum state_return_code (*StateFunction)(const char input, struct file_info* file_data);

enum state_return_code start_state(const char input, struct file_info* file_data)
{
	if(input == 'P') return RC_TRANSITION;
	else return RC_INVALID;
}

enum state_return_code type_state(const char input, struct file_info* file_data)
{
	static byte digit = 0;
	static char buffer[2];
	if(!digit)
	{
		if(is_digit(input))
		{
			digit = 1;
			buffer[0] = input;
			return RC_REPEAT;
		}
		else return RC_INVALID;
	}
	else if(digit)
	{
		if(is_whitespace(input))
		{
			buffer[1] = 0;
			file_data->type = (byte)atoi(buffer);
			return RC_TRANSITION;
		}
		else return RC_INVALID;
	}
	return RC_INVALID;
}

enum state_return_code info_number_state(const char input, struct file_info* file_data)
{
	static byte is_whitespace_valid = 0;
	static uint target_data_field_index = 0;
	static uint* target_data_field;
       	target_data_field = &(file_data->width) + target_data_field_index;
	static char buffer[8];
	static char number_char_pos = 0;
	if(!is_whitespace_valid)
	{
		for(uint i = 0; i < 8; i++) buffer[i] = 0;
		if(is_digit(input))
		{
			is_whitespace_valid = 1;
			buffer[number_char_pos] = input;
			number_char_pos++;
			return RC_REPEAT;
		}
		else return RC_INVALID;
	}
	else
	{
		if(is_digit(input)) 
		{
			buffer[number_char_pos] = input;
			number_char_pos++;
			return RC_REPEAT;
		}
		else if(is_whitespace(input)) 
		{
			*target_data_field = (uint)atoi(buffer);
			for(uint i = 0; i < 8; i++) buffer[i] = 0;
			number_char_pos = 0;
			is_whitespace_valid = 0;
			target_data_field_index++;
			if(target_data_field_index == 2) allocate_colour_data(file_data);
			return RC_TRANSITION;
		}
		else return RC_INVALID;
	}
	return RC_INVALID;
}

enum state_return_code colour_number_state(const char input, struct file_info* file_data)
{
	static byte is_whitespace_valid = 0;
	static char buffer[8];
	static uint colour_number = 0;
	static uint number_char_pos = 0;
	static uint* colour_data_field;
       	colour_data_field = file_data->colour_vals + colour_number;
	if(!is_whitespace_valid)
	{
		for(uint i = 0; i < 8; i++) buffer[i] = 0;
		if(is_digit(input))
		{
			is_whitespace_valid = 1;
			buffer[number_char_pos] = input;
			number_char_pos++;
			return RC_REPEAT;
		}
		else if(input == 0) return RC_END;
		else return RC_INVALID;
	}
	else
	{
		if(is_digit(input))
		{
			buffer[number_char_pos] = input;
			number_char_pos++;
			return RC_REPEAT;
		}
		else if(is_whitespace(input))
		{
			*colour_data_field = (uint)atoi(buffer);
			colour_number++;
			for(uint i = 0; i < 8; i++) buffer[i] = 0;
			number_char_pos = 0;
			is_whitespace_valid = 0;
			return RC_TRANSITION;
		}
		else return RC_INVALID;
	}
	return RC_INVALID;
}

const StateFunction state_functions[] =
{
	start_state, type_state, info_number_state, colour_number_state
};

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

const struct transition transitions[9] = 
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


void gohere()
{
	printf("THERE\n");
}

byte parsePPM(const char* ppm, struct file_info* file_data)
{
	uint str_length = strlen(ppm);
	uint current_char_pos = 0;
	const char* current_char = ppm;
	enum parse_state p_state = P_STATE_START;
	enum parse_state before_invalid_state = P_STATE_INVALID;
	enum state_return_code rc;
	StateFunction current_state_func = lookup_state_function(p_state);
	uint c = 131800;
	uint d = 129440 / sizeof(uint);
	printf("HERE %d\n", d);
	for(; p_state != P_STATE_END && p_state != P_STATE_INVALID; current_char++, current_char_pos++)
	{
		if(current_char_pos >= c) 
		{
			for(uint i = 0; i < 6; i++)
			{
				gohere();
			}
			printf("HERE\n");
		}
		rc = current_state_func(*current_char, file_data);
		switch(rc)
		{
			case RC_TRANSITION:
				p_state = transition_state(p_state, rc);
				current_state_func = lookup_state_function(p_state);
				break;
			case RC_INVALID:
				before_invalid_state = p_state;
				p_state = P_STATE_INVALID;
				break;
			case RC_END:
				p_state = P_STATE_END;
				break;
			default:
				break;
		}
	}
	if(p_state == P_STATE_END) return 1;
	else
	{
		printf("Invalid in state %d at char %d = %d\n", before_invalid_state, current_char_pos, *current_char);
		return 0;
	}
}

byte processFile(const char* file_path)
{
	FILE* file = fopen(file_path, "r");
	if(!file)
	{
		printf("Error: Could not open %s!\n", file_path);
		return -1;
	}
	
	//Read file here
	uint file_size;
	fseek(file, 0, SEEK_END);
	file_size = ftell(file) + 1;
	fseek(file, 0, SEEK_SET);
	printf("%s size = %d\n", file_path, file_size);
	char* file_contents = malloc(file_size);
	fread(file_contents, 1, file_size, file);
	fclose(file);
	file_contents[file_size-1] = 0;
	struct file_info file_data;
	byte ret_val = parsePPM(file_contents, &file_data);
	printf("Parsed = %d\n", ret_val);
	printf("Type = %d\n", file_data.type);
	printf("Width = %d\n", file_data.width); 
	printf("Height = %d\n", file_data.height); 
       	printf("Max = %d\n", file_data.max_val);
	free(file_contents);
	free(file_data.colour_vals);
	return 0;
}

int main(int argc, char** argv)
{
	if(argc <= 1)
	{
		printf("Error: No file specified!\n");
		return 1;
	}
	else
	{
		return processFile(argv[1]);
	}
}
