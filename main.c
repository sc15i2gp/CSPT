#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Pattern.h"

//END RESULT: Program takes an image, creates a ppm by interpolating areas of colour, outputs a cross stitch pattern from this

//TODO: Floss codes
//	Floss codes are DMC
//	Already have map of colours => symbols
//	Need map of symbols => flosses
//	Therefore also need map of colours => flosses
//	
//	For each colour in colour => symbol map:
//		Print symbol => floss pair
//
//	Include symbol to DMC list on pattern

//TODO: Put symbols => flosses map on output pattern


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
