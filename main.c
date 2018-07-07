#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Pattern.h"

//END RESULT: Program takes an image, creates a ppm by interpolating areas of colour, outputs a cross stitch pattern from this

//TODO: Restrict use of file_info struct and replace w/ uint* use where applicable
//	Have symbols list store uint* instead of file_info
//	Rename file_info to ppm_file_data

//TODO: Create symbols => floss map image

//TODO: Composite symbols pattern and map image into one output image

//NOTE: Useful output info
//	Number of stitches in a file
//	Number of colours in a file
//	Number of stitches of a particular colour in a file

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
