#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tokenize.h"
#include "linked_list.h"

#define clr_whitespace(x, y) while(isspace(x[y])) y++;

static char *alloc_and_copy(char *in, int end);
static int next_token(char *in, ll_t *tokens);
static void tokenize_error(char *message);

// Tokenize a whole file
ll_t *tokenize_file(FILE *infile) {
	int used, i;
	size_t len;
	char *line = NULL;
	ll_t *result = new_ll();

	while (getline(&line, &len, infile) != -1) {
		used = 0;
		i = 0;

		clr_whitespace(line, i);
		while ((used = next_token(line + i, result)) != 0) {
			i += used;
			clr_whitespace(line, i);
		}
		free(line);
		line = NULL;
	}
	free(line);
	return result;
}

// Alloc and copy a string, just for sanity sake
static char *alloc_and_copy(char *in, int end) {
	int len = strlen(in);

	// Just alloc end or length, whatever is smaller
	char *result = calloc((len < end ? len : end) + 1, sizeof(char));
	strncpy(result, in, end);
	return result;
}

static int next_token(char *in, ll_t *tokens) {
	int num_used = 1;
	// TODO Strip whitespace

	// Check if at end
	if (in[0] == 0) {
		return 0;
	}

	// TODO: More token types
	if (strncmp(in, "return", 6) == 0) {
		ll_add(tokens, alloc_and_copy("return", 7));
		num_used = 6;
	} else if (strncmp(in, "while", 5) == 0) {
		ll_add(tokens, alloc_and_copy("while", 6));
		num_used = 5;
	} else if (strncmp(in, "else", 4) == 0) {
		ll_add(tokens, alloc_and_copy("else", 5));
		num_used = 4;
	} else if (strncmp(in, "if", 4) == 0) {
		ll_add(tokens, alloc_and_copy("if", 3));
		num_used = 2;
	} else if (strncmp(in, "==", 2) == 0) {
		ll_add(tokens, alloc_and_copy("==", 3));
		num_used = 2;
	} else if (strncmp(in, "!=", 2) == 0) {
		ll_add(tokens, alloc_and_copy("!=", 3));
		num_used = 2;
	} else if (strncmp(in, "<=", 2) == 0) {
		ll_add(tokens, alloc_and_copy("<=", 3));
		num_used = 2;
	} else if (strncmp(in, ">=", 2) == 0) {
		ll_add(tokens, alloc_and_copy(">=", 3));
		num_used = 2;
	} else if (strncmp(in, "||", 2) == 0) {
		ll_add(tokens, alloc_and_copy("||", 3));
		num_used = 2;
	} else if (in[0] == ';') {
		ll_add(tokens, alloc_and_copy(";", 2));
	} else if (in[0] == '=') {
		ll_add(tokens, alloc_and_copy("=", 2));
	} else if (in[0] == '(') {
		ll_add(tokens, alloc_and_copy("(", 2));
	} else if (in[0] == ')') {
		ll_add(tokens, alloc_and_copy(")", 2));
	} else if (in[0] == '{') {
		ll_add(tokens, alloc_and_copy("{", 2));
	} else if (in[0] == '}') {
		ll_add(tokens, alloc_and_copy("}", 2));
	}

	// Done with easy
	else if (in[0] == '\"') {
		// String
		// TODO: EOF checking
		num_used = strcspn(in+1, "\"") + 2; // Plus one for in+1, and one for idex -> len conversion

		if (num_used <= 0) {
			tokenize_error("Never found a closing quote!");
		}

		ll_add(tokens, alloc_and_copy(in, num_used));
	} else if (isdigit(in[0])) {
		// Number
		// Really just get length of next int

		// TODO: Quadratic time issue
		sscanf(in, "%*i%n", &num_used);
		ll_add(tokens, alloc_and_copy(in, num_used));
	} else {
		// Must be identifier
		printf("Found Ident\n");
		char *res = calloc(sizeof(char), 32);

		// Reading character limited strings makes me want to cry 
		//       👁️ 👄 👁️    (PS: Bless Unicode for 
		//         💧  💧        making that possible)
		sscanf(in, "%31[abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_]%n", res, &num_used);
		if (strlen(res) > 0) // Call strlen vs num_used bc of scanf newline eating
			ll_add(tokens, res);
		else
			free(res);
	}
	return num_used;
} 

// Should be a very rare case
static void tokenize_error(char *message) {
	fprintf(stderr, "%s\n", message);
	exit(1);
}