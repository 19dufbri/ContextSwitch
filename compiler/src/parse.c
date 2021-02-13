#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linked_list.h"

typedef enum {
	OPEN_P,  	//
	CLOSE_P, 	//
	STRING,		//
	NUMBER,		//
	VAR,		//
	FUN,		//
	OPEN_B, 	//
	CLOSE_B, 	//
	IDENT, 		//
	RETURN,		//
	SEMICOLON,	//
	EQUALS		//
} TokenType_t;

typedef struct {
	TokenType_t type;
	union {
		char *string;
		uint16_t number;
	} value;
} Token_t;

typedef enum {
	FILE,
	VAR,
	FUN,
	LITERAL
} ParseType_t;

typedef struct {
	ParseType_t type;
	char *name;
	union {
		Parse_t *child;
		ll_t *children;
	} value;
} Parse_t;

void syntax_error(char *message);
Parse_t *parse_file(ll_t *tokens);

ll_t *tokens;
void tokenize(FILE *infile);
int get_token(char *in);

int main(int argc, char *argv[]) {
	if (argc < 2) {
		return -1;
	}

	tokens = new_ll();

	FILE *infile = fopen(argv[1], "r");
	if (infile == 0) {
		fprintf(stderr, "IO Error\n");
		return -1;
	}
	printf("B4 Tokenize\n");
	tokenize(infile);
	fclose(infile);
	printf("After Tokenize\n");

	ll_iter_rewind(tokens);
	Token_t *c_tok;
	while ((c_tok = ll_iter_next(tokens)) != NULL) {
		printf("Token Type %d\n", c_tok->type);
		if (c_tok->type == IDENT) {
			printf("  Value: %s\n", c_tok->value.string);
		}
	}

	parse_file(tokens);

	return 0;
}

Parse_t *parse_file(ll_t *tokens) {
	Token_t *token;
	ll_iter_rewind(tokens);

	while ((token = ll_iter_next(tokens)) != NULL) {
		switch (token->type) {
			case VAR:
				printf("Got VAR\n");
				break;
			case FUN:
				printf("Got FUN\n");
				break;
			default:
				fprintf(stderr, " >> %d <<\n", token->type);
				syntax_error("Expected `var` or `func`");
				break;
		}
	}

	return NULL;
}

Parse_t *parse_var(ll_t *tokens) {
	Token_t *token = ll_iter_next(tokens);

	if (token->type != IDENT) {
		fprintf(" >> %d <<\n", token->type);
		syntax_error("Expected variable name!");
	}

	Parse_t *result = malloc(sizeof(Parse_t));
	result->type = VAR;
	result->name = token->value.string;

	token = ll_iter_next(tokens);
	switch (token->type) {
		case EQUALS:
			result->value.child = parse_rvalue(tokens);
			break;
		case SEMICOLON:
			break;
		default:
			fprintf(stderr, " >> %d <<\n", token->type);
			syntax_error("Expected `=` or `;`");
			break;
	}

	return result;
}

Parse_t *parse_rvalue(ll_t *tokens) {

}

void tokenize(FILE *infile) {
	int idex, i;
	size_t len;
	char *line = NULL;

	while (getline(&line, &len, infile) != -1) {
		idex = 0;
		i = 0;
		while(isspace(line[i])) i++;
		//printf("Char b4 ");
		while (idex = get_token(line + i)) {
			i += idex;
			while(isspace(line[i])) i++;
		}
		free(line);
		line = NULL;
	}
}

int get_token(char *in) {
	int num_used = 1;
	// TODO Strip whitespace
	if (strlen(in) == 0) {
		return 0;
	}

	if (in[0] == ';') {
		Token_t *newTok = malloc(sizeof(Token_t));
		newTok->type = SEMICOLON;
		ll_add(tokens, newTok);
	} else if (in[0] == '=') {
		Token_t *newTok = malloc(sizeof(Token_t));
		newTok->type = EQUALS;
		ll_add(tokens, newTok);
	} else if (in[0] == '(') {
		Token_t *newTok = malloc(sizeof(Token_t));
		newTok->type = OPEN_P;
		ll_add(tokens, newTok);
	} else if (in[0] == ')') {
		Token_t *newTok = malloc(sizeof(Token_t));
		newTok->type = CLOSE_P;
		ll_add(tokens, newTok);
	} else if (in[0] == '{') {
		Token_t *newTok = malloc(sizeof(Token_t));
		newTok->type = OPEN_B;
		ll_add(tokens, newTok);
	} else if (in[0] == '{') {
		Token_t *newTok = malloc(sizeof(Token_t));
		newTok->type = CLOSE_B;
		ll_add(tokens, newTok);
	} else if (strncmp(in, "var", 3) == 0) {
		Token_t *newTok = malloc(sizeof(Token_t));
		newTok->type = VAR;
		ll_add(tokens, newTok);
		num_used = 3;
	} else if (strncmp(in, "fun", 3) == 0) {
		Token_t *newTok = malloc(sizeof(Token_t));
		newTok->type = FUN;
		ll_add(tokens, newTok);
		num_used = 3;
	} else if (strncmp(in, "return", 6) == 0) {
		Token_t *newTok = malloc(sizeof(Token_t));
		newTok->type = RETURN;
		ll_add(tokens, newTok);
		num_used = 6;
	}

	// Done with easy
	else if (in[0] == '\"') {
		// String
		Token_t *newTok = malloc(sizeof(Token_t));
		newTok->type = STRING;

		int idex = strcspn(in+1, "\"");
		newTok->value.string = calloc(sizeof(char), idex);
		strncpy(newTok->value.string, in+1, idex-1);
		newTok->value.string[idex-1] = '\0';

		ll_add(tokens, newTok);
		num_used = idex+1;
	} else if (isdigit(in[0])) {
		// Number
		Token_t *newTok = malloc(sizeof(Token_t));
		newTok->type = NUMBER;

		sscanf(in, "%hi%n", &newTok->value.number, &num_used);
		ll_add(tokens, newTok);
	} else {
		// Must be identifier
		Token_t *newTok = malloc(sizeof(Token_t));
		newTok->type = IDENT;

		newTok->value.string = calloc(sizeof(char), 32);

		// Reading character limited strings makes me want to cry
		sscanf(in, "%31[abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_]%n", newTok->value.string, &num_used);

		ll_add(tokens, newTok);
	}
	return num_used;
} 

void syntax_error(char *message) {
	fprintf(stderr, "%s\n", message);
	exit(1);
}