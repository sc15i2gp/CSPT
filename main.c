#include <stdio.h>

//I want to specify a file name and the program should read it:
//Outputting its width and height
//	Open file with given file name

typedef unsigned char byte;

byte processFile(const char* filePath)
{
	FILE* file = fopen(filePath, "r");
	if(!file)
	{
		printf("Error: Could not open %s!\n", filePath);
		return -1;
	}
	
	//Read file here

	fclose(file);
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
