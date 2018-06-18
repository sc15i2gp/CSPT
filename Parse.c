#include "Parse.h"


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
	//Input string data
	uint str_length = strlen(ppm);
	uint current_char_pos = 0;
	const char* current_char = ppm;
	
	//State data
	enum parse_state p_state = P_STATE_START;
	enum parse_state before_invalid_state = P_STATE_INVALID;
	enum state_return_code rc;
	struct parse_state_data state_data;
		
	StateFunction current_state_func;
	prepare_state(p_state, &state_data, &current_state_func);
	
	for(; p_state != P_STATE_END && p_state != P_STATE_INVALID; current_char++, current_char_pos++)
	{
		set_target_field(p_state, &state_data, file_data);
		rc = current_state_func(*current_char, &state_data);
		switch(rc)
		{
			case RC_TRANSITION:
				p_state = transition_state(p_state, rc);
				if(p_state == P_STATE_MAX) allocate_colour_data(file_data);
				prepare_state(p_state, &state_data, &current_state_func);
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

