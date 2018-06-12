#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Parse.h"

int main(int argc, char** argv)
{
	if(argc <= 1)
	{
		printf("Error: No file specified!\n");
		return 1;
	}
	else
	{
		struct file_info* file_data = process_file(argv[1]);
		if(file_data)
		{
			byte b = file_data->type == 3;
			destroy_file(file_data);
			return b;
		}
		else return -1;
	}
}
