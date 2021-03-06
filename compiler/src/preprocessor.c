#include <stdio.h>
#include <string.h>

#include "tokenize.h"
#include "linked_list.h"

static char *eat_tok_pre(ll_t *tokens);
static void found_pre(ll_t *tokens);
static void preprocess_error(char *message);

// Preprocess all the tokens in a list and clear the list
char **preprocess(ll_t *tokens, ssize_t *length) {
	char *token;

	// Normal preprocesser tings
	ll_iter_rewind(tokens);
	while ((token = ll_iter_next(tokens)) != NULL) {
		// Check if preprocessor directive
		if (strcmp("#", token) == 0) {
			ll_remove_iter(tokens); // Remove '#' from tokens
			free(token);
			found_pre(tokens);
		}
	}
	
	// Move everything over to the array
	*length = ll_len(tokens);
	char **result = calloc(*length, sizeof(char *));
	ll_iter_rewind(tokens);
	int i = 0;
	while ((token = ll_iter_next(tokens)) != NULL) {
		result[i++] = token;
	}

	return result;
}

// Get token and remove it from the iterator, normal operation
static char *eat_tok_pre(ll_t *tokens) {
	char *token = ll_iter_next(tokens);
	if (token == NULL) {
		preprocess_error("Unexpected EOF!");
	}
	ll_remove_iter(tokens); // Remove token
	return token;
}

// Found a preprocessor directive
static void found_pre(ll_t *tokens) {
	char *token = eat_tok_pre(tokens);
	
	if (strcmp(token, "include") == 0) {
		// Include statement, followed by file type
		token = eat_tok_pre(tokens);

		// Header file
		// Load contents of new file
		FILE *infile = fopen(token, "r");
		if (infile == NULL) {
			preprocess_error("Tried to include bad file!");
		}
		ll_t *n_tokens = tokenize_file(infile);
		fclose(infile);
		// Done loading

		// Merge token lists
		while ((token = ll_iter_next(n_tokens)) != NULL) {
			ll_add_iter(tokens, token);
		}

		// TODO: Make it not n^k, only n (because every time an include, will start over currently)
		ll_iter_rewind(tokens);
	}
}

static void preprocess_error(char *message) {
	fprintf(stderr, "Error in preprocessor:\n");
	fprintf(stderr, "\t>> %s <<\n", message);
	exit(1);
}