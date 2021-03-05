#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tokenize.h"
#include "linked_list.h"

int main(int argc, char *argv[]) {
	// TODO: Better checking
	if (argc < 2) {
		printf("no.\n");
		return -1;
	}

	// Inital tokenization stage
	FILE *infile = fopen(argv[1], "r");
	if (infile == 0) {
		fprintf(stderr, "IO Error\n");
		return -1;
	}

	ll_t *tokens_list = tokenize_file(infile);
	fclose(infile);

	ll_iter_rewind(tokens_list);
	// End inital tokenization
	
	// Preprocessor
	char *tokens_array[] = preprocess(tokens_list);
	del_ll(tokens_list); // Cleared by preprocess, can now free
	// End preprocessor

	// Free them! 
	// TODO: Valgrind
	free(tokens_array);
	
	return 0;
}