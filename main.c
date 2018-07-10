#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Pattern.h"

//END RESULT: Program takes an image, creates a ppm by interpolating areas of colour, outputs a cross stitch pattern from this

//TODO: Tidy up code
//	Unify symbols and glyphs functions
//	Make glyphs code more general (could handle more letters if needs be)
//	Move map printing to map module
//	Move counting total stitch
//	Remove duplicate code (like count_colours_in_file)
//	Could store ref and contents_type in one uint instead of struct
//	Remove as much duplicate code as is possible in floss_to_symbol_pattern stuff
//	See if it is feasible to unify uint* copying code which is repeated 3 times in various functions (such as in create_floss_to_symbol_pattern_map)
//	Split down things into more functions
//	Make general pattern_info and ppm_file_data creation/destruction functions
//	Make pattern_info printing function
//IDEA: Could make general file interaction code and PPM specific code separate

//TODO: Logger
//	Debug, info, warning, error levels
//	Can choose destination

//TODO: Make sure program outputs everything it does
//TODO: Make sure pattern info is properly logged, including src image and final image paths

//TODO: Determine if it is feasible to use latex to generate a pdf
//	Latex used to lay out pattern, pattern map and pattern info
//	pdf generated 

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
