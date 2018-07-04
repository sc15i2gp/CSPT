#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Pattern.h"

//END RESULT: Program takes an image, creates a ppm by interpolating areas of colour, outputs a cross stitch pattern from this

//NOTE: Segfault on i = 44 when loading DMC floss map, due to being unable to find pair in get_pair()

//TODO: Fix above segfault:
//	Issue is due to erroneous pointer setting (parent ptrs pointing to grandparent etc.)
//	Make standard function set_child(node n, node c, bool left) which not only sets n->child but also sets c->parent


//TODO: Make load_symbols place white first and black second in symbols_list

//TODO: Make rb_tree more generic

//TODO: web scrape DMC floss code list and place into .c file

//TODO: Map of colour => floss code

//TODO: Map of symbol index => floss code

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
