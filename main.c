#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Pattern.h"

//END RESULT: Program takes an image, creates a ppm by interpolating areas of colour, outputs a cross stitch pattern from this

//NOTE: Output pdf
//	single text file which is a string of a latex file, with python's .format() in mind
//	python script formats it from output of this program

//TODO: Have python program save html once then use that file

//TODO: Use makefile to run DMC generation before building c files

//TODO: Output p_info to file for use in generating pdf


//TODO: Logger
//	Debug, info, warning, error levels
//	Can choose destination

//TODO: Make sure enough symbols are loaded to meet the needs of the colours_to_symbols map
//TODO: Make sure program outputs everything it does
//TODO: Make sure pattern info is properly logged, including src image and final image paths

//TODO: Make it so that output pattern can be separated into pages, where a bit of the pattern is shown

//NOTE: Useful output info
//	Number of stitches in a file
//	Number of colours in a file
//	Number of stitches of a particular floss in a file
//	Width in stitches of a file
//	Height in stitches of a file

int main(int argc, char** argv)
{
	if(argc <= 1)
	{
		printf("Error: No file specified!\n");
		return 2;
	}
	else
	{
		byte pattern_created = create_pattern(argv[1]);
		return !pattern_created;
	}
}
