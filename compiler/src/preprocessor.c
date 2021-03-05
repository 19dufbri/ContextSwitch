#include "linked_list.h"

char **preprocess(ll_t *tokens) {
	char *token;

	ll_iter_rewind(tokens);
	while ((token = ll_iter_next(tokens)) != NULL) {
		// Check if preprocessor directive
		if (strcmp("#", token) == 0) {
			ll_remove_iter(tokens); // Remove '#' from tokens
			free(token);
			found_pre(tokens);
		}
	}
}

static char *get_t_pre(tokens) {
	char *token = ll_iter_next(tokens);
	if (token == NULL) {
		preprocess_error("Unexpected EOF!");
	}
	ll_remove_iter(tokens); // Remove token
	return token;
}

// Found a preprocessor directive
static void found_pre(tokens) {
	char *token = get_t_pre(tokens);
	
	if (strcmp(token, "include") == 0) {
		// Include statement, followed by file string
		token = get_t_pre(tokens);
		
		// Load contents of new file
		FILE *infile = fopen(token, "r");
		if (infile == NULL) {
			preprocess_error("Tried to include bad file!");
		}
		ll_t *n_tokens = tokenize(infile);
		fclose(infile);

		// Merge token lists
	}
}

static void preprocess_error(char *message) {
	fprintf(stderr, "Error in preprocessor:\n");
	fprintf(stderr, "\t>> %s <<\n", message);
	exit(1);
}