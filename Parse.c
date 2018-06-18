#include "Parse.h"

void allocate_colour_data(struct file_info* file_data)
{
	uint number_of_colour_vals = file_data->width * file_data->height;
	printf("Allocating %d pixles of uints\n", number_of_colour_vals);
	file_data->colour_vals = malloc(3 * number_of_colour_vals * sizeof(uint));
}

void destroy_file(struct file_info* file_data)
{
	if(file_data->colour_vals) free(file_data->colour_vals);
	free(file_data);
}

void print_to_char_address(int argc, ...)
{
	va_list valist;
	va_start(valist, argc);
	//First arg is str location
	char** str_loc = va_arg(valist, char**);
	const char* format = va_arg(valist, const char*);
	uint added = sprintf(*str_loc, format, valist);
	*str_loc += added;
	va_end(valist);
}

void print_to_file(const char* file_name, const char* to_print)
{
	FILE* file = fopen(file_name, "w");
	fprintf(file, "%s", to_print);
	fclose(file);
}

void print_to_ppm(const char* file_name, struct file_info* file_data)
{
	printf("Printing to ppm: %s...\n", file_name);
	uint file_size = 2+1 //first line 'P3'
			+16+2 //second line width, height
			+8+1 //third line max_value
			+(16 * 3 * file_data->width*file_data->height) //size of each number
			+(3 * file_data->width*file_data->height); //number of whitespace chars
	char* to_print = malloc(file_size);
	char* current_char = to_print;
	*current_char = 'P';
	current_char++;
	current_char += sprintf(current_char, "%u\n", file_data->type);
	current_char += sprintf(current_char, "%u %u\n", file_data->width, file_data->height);
	current_char += sprintf(current_char, "%u\n", file_data->max_val);

	for(uint i = 0; i < file_data->height; i++)
	{
		for(uint j = 0; j < file_data->width * 3; j++)
		{
			uint current_val = *(file_data->colour_vals + (i*3*file_data->width) + j);
			current_char += sprintf(current_char, "%u\n", current_val);
		}
	}
	print_to_file(file_name, to_print);
	free(to_print);
	printf("Done printing\n");
}

struct file_info* process_file(const char* file_path)
{
	FILE* file = fopen(file_path, "r");
	if(!file)
	{
		printf("Error: Could not open %s!\n", file_path);
		return NULL;
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
	struct file_info* file_data = malloc(sizeof(struct file_info));
	byte ret_val = parsePPM(file_contents, file_data);
	printf("Parsed = %d\n", ret_val);
	printf("Type = %d\n", file_data->type);
	printf("Width = %d\n", file_data->width); 
	printf("Height = %d\n", file_data->height); 
       	printf("Max = %d\n", file_data->max_val);
	free(file_contents);
	
	return (ret_val) ? file_data : NULL;
}

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

enum state_return_code start_state(const char input, struct parse_state_data* state_info, struct file_info* file_data)
{
	if(input == 'P') return RC_TRANSITION;
	else return RC_INVALID;
}

//Type is a single digit, state takes a single digit and a whitespace
//State data:
//	byte for whether the digit has been read
//	char buffer
enum state_return_code type_state(const char input, struct parse_state_data* state_info, struct file_info* file_data)
{
	if(!state_info->is_whitespace_valid)
	{
		if(is_digit(input))
		{
			state_info->is_whitespace_valid = 1;
			set_state_buffer_char(state_info, input);
			return RC_REPEAT;
		}
		else return RC_INVALID;
	}
	else
	{
		if(is_whitespace(input))
		{
			set_state_buffer_char(state_info, 0);
			file_data->type = (byte)atoi(state_info->buffer);
			return RC_TRANSITION;
		}
		else return RC_INVALID;
	}
	return RC_INVALID;
}


//Number state takes up to 8 digit chars, and a whitespace
//State data:
//	byte to track to make sure at least one digit has been read (is_whitespace_valid)
//	uint for which data field in file_info to write number to
//	uint* for the address of data field
//	char buffer for read digits
//	char for which position in char buffer to write digit char to 
enum state_return_code info_number_state(const char input, struct parse_state_data* state_info, struct file_info* file_data)
{
	if(!state_info->is_whitespace_valid)
	{
		clear_state_char_buffer(state_info);
		if(is_digit(input))
		{
			state_info->is_whitespace_valid = 1;
			set_state_buffer_char(state_info, input);
			return RC_REPEAT;
		}
		else return RC_INVALID;
	}
	else
	{
		if(is_digit(input)) 
		{
			set_state_buffer_char(state_info, input);
			return RC_REPEAT;
		}
		else if(is_whitespace(input)) 
		{
			*(state_info->file_info_field.uint_field) = (uint)atoi(state_info->buffer);
			if(state_info->file_info_field.uint_field == &(file_data->max_val)) allocate_colour_data(file_data); //If width and height have been read, allocate space for colour values
			return RC_TRANSITION;
		}
		else return RC_INVALID;
	}
	return RC_INVALID;
}

//Number state reads colours values for pixels in ppm
//State data:
//	byte is_whitespace_valid
//	char buffer
//	uint for which index in the file_info's colour values array to write contents of char buffer into
//	uint for position in char buffer to write digit char to
//	uint* which points to index in file_info's colour values array
enum state_return_code colour_number_state(const char input, struct parse_state_data* state_info, struct file_info* file_data)
{
	if(!state_info->is_whitespace_valid)
	{
		clear_state_char_buffer(state_info);
		if(is_digit(input))
		{
			state_info->is_whitespace_valid = 1;
			set_state_buffer_char(state_info, input);
			return RC_REPEAT;
		}
		else if(input == 0) return RC_END;
		else return RC_INVALID;
	}
	else
	{
		if(is_digit(input))
		{
			set_state_buffer_char(state_info, input);
			return RC_REPEAT;
		}
		else if(is_whitespace(input))
		{
			*(state_info->file_info_field.uint_field) = (uint)atoi(state_info->buffer);
			state_info->data_field_pos++;
			return RC_TRANSITION;
		}
		else return RC_INVALID;
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

void set_target_field(enum parse_state current_state, struct parse_state_data* state_info, struct file_info* file_data)
{
	switch(current_state)
	{
		case P_STATE_TYPE:
			state_info->file_info_field.byte_field = &(file_data->type);
			break;
		case P_STATE_WIDTH:
			state_info->file_info_field.uint_field = &(file_data->width);
			break;
		case P_STATE_HEIGHT:
			state_info->file_info_field.uint_field = &(file_data->height);
			break;
		case P_STATE_MAX:
			state_info->file_info_field.uint_field = &(file_data->max_val);
		       break;
		case P_STATE_R:
		case P_STATE_G:
		case P_STATE_B:
			state_info->file_info_field.uint_field = file_data->colour_vals + state_info->data_field_pos;
		 	break;
		default:
			break;	
	}
}

byte parsePPM(const char* ppm, struct file_info* file_data)
{
	uint str_length = strlen(ppm);
	uint current_char_pos = 0;
	const char* current_char = ppm;
	enum parse_state p_state = P_STATE_START;
	enum parse_state before_invalid_state = P_STATE_INVALID;
	enum state_return_code rc;
	struct parse_state_data state_data;
	reset_parse_state_data(&state_data);
	StateFunction current_state_func = lookup_state_function(p_state);
	for(; p_state != P_STATE_END && p_state != P_STATE_INVALID; current_char++, current_char_pos++)
	{
		set_target_field(p_state, &state_data, file_data);
		rc = current_state_func(*current_char, &state_data, file_data);
		switch(rc)
		{
			case RC_TRANSITION:
				p_state = transition_state(p_state, rc);
				current_state_func = lookup_state_function(p_state);
				reset_parse_state_data(&state_data);
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

