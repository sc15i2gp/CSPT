#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Pattern.h"

//END RESULT: Program takes an image, creates a ppm by interpolating areas of colour, outputs a cross stitch pattern from this

//NOTE: Output pdf
//	single text file which is a string of a latex file, with python's .format() in mind
//	python script formats it from output of this program

//TODO: Output p_info to file for use in generating pdf

//TODO: Logger
//	Debug, info, warning, error levels
//	Can choose destination

//TODO: Make sure enough symbols are loaded to meet the needs of the colours_to_symbols map
//TODO: Make sure program outputs everything it does
//TODO: Make sure pattern info is properly logged, including src image and final image paths

//TODO: Make it so that output pattern can be separated into pages, where a bit of the pattern is shown

//TODO: Have program gracefully fail
//	When input file doesn't exist


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
		const char* src_img_path = argv[1];
		uint page_stitch_length = (argc > 2) ? atoi(argv[2]) : 50;
		const char* output_dir = (argc > 3) ? argv[3] : "Output";
		printf("CONVERTING %s | OUTPUT_DIR %s/ | PAGE LENGTH IN STITCHES %d\n", src_img_path, output_dir, page_stitch_length);
		byte pattern_created = create_pattern(src_img_path, output_dir, page_stitch_length);
		return !pattern_created;
	}
}
