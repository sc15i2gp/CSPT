#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Pattern.h"

//END RESULT: Program takes an image, creates a ppm by interpolating areas of colour, outputs a cross stitch pattern from this

//TODO: Create symbols => floss map image
//	Output as ppm
//	Centered display
//	Left aligned titles "Symbol" and "DMC Floss"
//	Each line has symbol pic on left and floss code (digits) on right


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
