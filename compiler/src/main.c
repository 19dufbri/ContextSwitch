#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tokenize.h"
#include "preprocessor.h"
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
	ssize_t tokens_length;
	char **tokens_array = preprocess(tokens_list, &tokens_length);
	del_ll(tokens_list); // Cleared by preprocess, can now free
	// End preprocessor

	// Begin Parsing
	Parse_t *ast = parse(tokens_array);
	free(tokens_array);
	// End parsing

	// TODO: Valgrind
	return 0;
}