#include <stdio.h>
#include <stdlib.h>

//I want to specify a file name and the program should read it:
//Outputting its width and height
//	Open file with given file name

typedef unsigned char byte;
typedef unsigned int uint;

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
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	printf("%s size = %d\n", file_path, file_size);
	char* file_contents = malloc(file_size);
	fread(file_contents, 1, file_size, file);
	printf("%s\n", file_contents);
	fclose(file);



	free(file_contents);
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
