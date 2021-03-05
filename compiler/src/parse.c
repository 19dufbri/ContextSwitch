#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "linked_list.h"

typedef enum {
	OPEN_P,		// (			GOOD
	CLOSE_P,	// )			GOOD
	STRING,		// "..."		GOOD
	NUMBER,		// 0x00			GOOD
	OPEN_B,		// {			GOOD
	CLOSE_B,	// }			GOOD
	IDENT,		// identifier	GOOD
	RETURN,		// return 		GOOD
	SEMICOLON, 	// ;			GOOD
	EQUALS,		// =			GOOD
	DEQUALS,	// ==
	COMMA,		// ,
	IF,			// if 			GOOD
	ELSE,		// else 		GOOD
	FOR,		// for 			GOOD
	PLUS,		// +
	MINUS,		// -
	MULT,		// *
	DIV,		// /
	INCR,		// ++
	DECR,		// --
	XOR,		// ^
	DXOR,		// ^^
	OR,			// |
	DOR,		// ||
	AND,		// &
	DAND,		// &&
	LT,			// <
	GT,			// >
	LTE,		// <=
	GTE,		// >=
	NEQ,		// !=
	NOT			// !
	BLOCK,		// { ... }
	CONDITION,	// ( ... )
	VARDEF,
	FUNDEF
} TokenParseType_t;

typedef struct {
	TokenParseType_t type;
	union {
		char *string;
		uint16_t number;
	} value;
} Token_t;

typedef struct Parse {
	TokenParseType_t type;
	char *name;
	union {
		struct Parse *lchild;
		struct Parse *rchild;
		ll_t *children;
		char *string;
		uint16_t number;
	} value;
} Parse_t;

void syntax_error(char *message);
Parse_t *parse_file(ll_t *tokens);
Parse_t *parse_var(ll_t *tokens);
Parse_t *parse_rvalue(ll_t *tokens);

ll_t *tokens;
void tokenize(FILE *infile);
int get_token(char *in);

// Parse all statements in a file
Parse_t *parse_file(Token_t *tokens[], size_t len) {
	Token_t *token;
	int i = 0;

	while (i < len) {
		token = tokens[i++];
		switch (token->type) {
			case VAR:
				parse_var(tokens, &i, len); // TODO: Use result
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

// Parse a variable declaration
Parse_t *parse_var(Token_t *tokens[], int *i, size_t len) {
	Token_t *token = tokens[(*i)++];

	if (token->type != IDENT) {
		fprintf(stderr, " >> %d <<\n", token->type);
		syntax_error("Expected variable name!");
	}

	Parse_t *result = malloc(sizeof(Parse_t));
	result->type = VAR;
	result->name = token->value.string;

	token = tokens[(*i)++];
	switch (token->type) {
		case EQUALS:
			result->value.child = parse_rvalue(tokens, i, len);
			break;
		case SEMICOLON:
			result->value.child = NULL; // Signifier of no rvalue
			break;
		default:
			fprintf(stderr, " >> %d <<\n", token->type);
			syntax_error("Expected `=` or `;`");
			break;
	}

	return result;
}

// Parse "right side" of equals, also a value expression
Parse_t *parse_rvalue(Token_t *tokens[], int *i, size_t len) {
	// Expects only int or string for the moment
	Token_t *token = tokens[(*i)++];

	Parse_t *result = malloc(sizeof(Parse_t));

	// See if variable, or int/string literal
	// TODO : More complex rvalues
	switch (token->type) {
		case STRING:
			result->type = STRLIT;
			result->value.string = token->value.string;
			break;
		case NUMBER:
			result->type = INTLIT;
			result->value.number = token->value.number;
			break;
		case IDENT:
			result->type = PIDENT;
			result->value.string = token->value.string;
			break;
		default:
			fprintf(stderr, " >> %d <<\n", token->type);
			syntax_error("rvalue parse error");
			break;
	}

	// Check for end of line
	if ((token = tokens[(*i)++])->type != SEMICOLON) {
		syntax_error("Expected semicolon");
	}

	return result;
}

void syntax_error(char *message) {
	fprintf(stderr, "%s\n", message);
	exit(1);
}