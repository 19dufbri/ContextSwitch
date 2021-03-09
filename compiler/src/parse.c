typedef struct {
	enum {
		MATH,
		VAR_DECLR,
		FUN_DECLR,
		VAR_ASSIGN,
		FUN_ASSIGN,
		COMP_UNIT
	} type;
	ll_t *children;
} Parse_t;

Parse_t *parse(char **tokens, ssize_t len) {
	int i = 0;
	while (i < len) {
		char *token = tokens[i];

		if (token)
	}
}

static Parse_t *parse_var_declr(char **tokens, int i, ssize_t len) {

}

static Parse_t *parse_fun_declr(char **tokens, int i, ssize_t len) {
	
}