#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tokenize.h"
#include "linked_list.h"

int main(int argc, char *argv[]) {
	// TODO: Better checking
	if (argc < 2) {
		return -1;
	}

	// Inital tokenization stage
	FILE *infile = fopen(argv[1], "r");
	if (infile == 0) {
		fprintf(stderr, "IO Error\n");
		return -1;
	}

	ll_t *tokens = tokenize(infile);
	fclose(infile);
	// End inital tokenization
	
	// Preprocessor
	size_t len = ll_len(tokens);
	Token_t *tok_arr[] = calloc(len, sizeof(Token_t *));

	ll_iter_rewind(tokens);
	Token_t *c_tok;
	while ((c_tok = ll_iter_next(tokens)) != NULL) {
		tok_arr[i] = c_tok;
	}
	// End preprocessor

	// Free them!
	del_ll(tokens);

	parse_file(tok_arr, len);

	// Free memory
	// TODO Free Items
	free(tok_arr);
	return 0;
}