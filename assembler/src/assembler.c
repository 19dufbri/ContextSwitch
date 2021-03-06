#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "assembler.h"
#include "linked_list.h"

ll_t *tokens;
ll_t *label_def;
ll_t *label_ref;
ll_t *output;
uint16_t *out_full;

int main(int argc, char *argv[]) {
	// Sanity check
	if (argc < 3) {
		fprintf(stderr, "Not Enough Arguments!\n");
		fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
		return 1;
	}

	// Read input file and tokenize it
	FILE *infile = fopen(argv[1], "r");
	tokens = new_ll();

	tokenize(tokens, infile);
	fclose(infile);

	// Put to start for processing
	ll_iter_rewind(tokens);

	// Initialize linked lists
	label_def = new_ll();
	label_ref = new_ll();
	output = new_ll();

	// Create unlinked instruction stream
	int done;
	do {
		done = proc_instr();
	} while (!done);

	// Create final array of instructions
	out_full = calloc(ll_len(output), sizeof(uint16_t));

	// Rewind iterators
	ll_iter_rewind(output);
	ll_iter_rewind(label_ref);

	// Get first elements
	uint16_t *inst = (uint16_t *) ll_iter_next(output);
	Label_t *c_ref = ll_iter_next(label_ref);

	// For every instruction, linking if needed
	int i = 0;
	Label_t *c_def;
	while (inst != NULL) {
		if (c_ref != NULL && c_ref->addr == i) {
			// Found Reference

			// Find associated definition
			ll_iter_rewind(label_def);
			c_def = ll_iter_next(label_def);
			while (strcmp(c_ref->name, c_def->name) != 0) {
				c_def = ll_iter_next(label_def);

				// Ran off the end, missing definition
				if (c_def == NULL) {
					printf(" >> %s <<\n", c_ref->name);
					syntax_error("Undefined Label");
				}
			}

			// Update associated address
			*inst |= (c_def->addr & c_ref->mask) >> c_ref->shft;

			c_ref = ll_iter_next(label_ref);
		}

		// Move instruction to final list
		out_full[i++] = *inst;

		// Get next instruction
		inst = (uint16_t *) ll_iter_next(output);
	}

	// Write the final program to a file
	FILE *ofile = fopen(argv[2], "wb");
	fwrite(out_full, 2, ll_len(output), ofile);
	fclose(ofile);

	free_memory();

	printf("Assembly Successful\n");

	exit(0);
}

// Generate the instruction for each line of the input
int proc_instr() {
	uint16_t result;
	char *token = next_token(); // Freed at end of proc_instr
	if (token == NULL) {
		return 1; // EOF, expected exit
	}

	// Assembler Directives
	if (strcmp(token, "#INCLUDE") == 0) {
		// Include a file
		// TODO: FIX SKETCH SOLUTION
		char *fname = ll_iter_next(tokens); // ~Should~ be a filename
		FILE *newfile = fopen(fname, "r"); // TODO: FIX FILE DANGER
		if (newfile == NULL) {
			fprintf(stderr, " >> %s <<\n", fname);
			syntax_error("File not found");
		}
		tokenize(tokens, newfile);
		free(next_token()); // Filename again, can ignore
		fclose(newfile);
	}
	// "Real" Instructions
	else if (strcmp(token, "ADD") == 0) {		// ADD r1, r2, r0
		result = 0x0000;

		result |= expect_reg() << 4;	// r1
		result |= expect_reg();			// r2
		result |= expect_reg() << 8;	// r0

		out_add(result);
	} else if (strcmp(token, "SUB") == 0) {	// SUB r1, r2, r0
		result = 0x1000;

		result |= expect_reg() << 4;	// r1
		result |= expect_reg();			// r2
		result |= expect_reg() << 8;	// r0
		
		out_add(result);
	} else if (strcmp(token, "LIL") == 0) {	// LIL i,  r0
		result = 0x2000;

		Parse_t *t = next_num_or_label();
		if (t->type == LABEL) {
			add_label_ref(ll_len(output), t->value.name, 0xFF, 0);
		} else {
			result |= t->value.number & 0xFF;
		}
		free(t);

		result |= expect_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "LIH") == 0) {	// LIH i,  r0
		result = 0x3000;

		Parse_t *t = next_num_or_label();
		if (t->type == LABEL) {
			add_label_ref(ll_len(output), t->value.name, 0xFF, 8);
		} else {
			result |= (t->value.number >> 8) & 0xFF;
		}
		free(t);

		result |= expect_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "STO") == 0) { // STO r0, r1
		result = 0x4000;
		result |= expect_reg() << 8;
		result |= expect_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "LOA") == 0) { // LOA r1, r0
		result = 0x5000;
		result |= expect_reg() << 4;
		result |= expect_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "MOV") == 0) { // MOV r1, r0
		result = 0x6000;
		result |= expect_reg() << 4;
		result |= expect_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "ADI") == 0) { // ADI i,  r0
		result = 0x7000;

		Parse_t *t = next_num_or_label();
		if (t->type == LABEL) {
			add_label_ref(ll_len(output), t->value.name, 0xFF, 0);
		} else {
			result |= t->value.number & 0xFF;
		}
		free(t);

		result |= expect_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "SKL") == 0) { // SKL r0, r1
		result = 0x8000;
		result |= expect_reg() << 8;
		result |= expect_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "INT") == 0) { // INT
		result = 0x9000;
		out_add(result);
	} else if (strcmp(token, "EIN") == 0) { // EIN
		result = 0xA000;
		out_add(result);
	} else if (strcmp(token, "IOR") == 0) { // IOR i, r0
		result = 0xB000;

		Parse_t *t = next_num_or_label();
		if (t->type == LABEL) {
			add_label_ref(ll_len(output), t->value.name, 0xFF, 0);
		} else {
			result |= t->value.number & 0xFF;
		}
		free(t);

		result |= expect_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "IOW") == 0) { // IOW r0, i
		result = 0xC000;
		result |= expect_reg() << 8;

		Parse_t *t = next_num_or_label();
		if (t->type == LABEL) {
			add_label_ref(ll_len(output), t->value.name, 0xFF, 0);
		} else {
			result |= t->value.number & 0xFF;
		}
		free(t);

		out_add(result);
	} 

	// Pseudo-Instructions
	else if (strcmp(token, "LIR") == 0) { // LIR ii, r0
		result = 0x2000;

		Parse_t *t = next_num_or_label();
		
		result |= expect_reg() << 8;

		if (t->type == LABEL) {
			add_label_ref(ll_len(output), t->value.name, 0xFF, 0);
			add_label_ref(ll_len(output)+1, t->value.name, 0xFF, 8);
			out_add(result);
			out_add(result | 0x1000);
			free(t->value.name);
		} else {
			out_add(result | (t->value.number & 0xFF));
			out_add(result | 0x1000 | ((t->value.number & 0xFF) >> 8));
		}
		free(t);
	} else if (strcmp(token, "JMP") == 0) { // JMP r1
		result = 0x6700;
		result |= expect_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "JSR") == 0) { // JSR r1
		out_add(0x6570); // MOV PC, R5
		out_add(0x7504); // ADI 5,  R5
		out_add(0x4560); // STO R5, SP
		out_add(0x7601); // ADI 1,  SP

		// MOV r1, PC
		result = 0x6700;
		result |= expect_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "PSH") == 0) { // PSH r0 
		result = 0x4060;

		result |= expect_reg() << 8;

		out_add(result); // STO r0, SP
		out_add(0x7601); // ADI 1,  SP
	} else if (strcmp(token, "POP") == 0) { // POP r0 
		out_add(0x76FF); // ADI -1, SP

		result = 0x5060;

		result |= expect_reg() << 8;

		out_add(result); // LOA SP, r0
		
	} else if (strcmp(token, "JGT") == 0) { // JGT r0, r1, r2
		// If r0 > r1, goto r2 
		
		// SKL r0, r1
		result = 0x8000;
		result |= expect_reg() << 8;
		result |= expect_reg() << 4;
		out_add(result);
		
		// MOV r2, PC
		result = 0x6700;
		result |= expect_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "RET") == 0) { // RET
		// POP return address and jump
		out_add(0x76FF); // ADI		-1, 	SP
		out_add(0x5760); // LOA 	SP,		PC
	}

	else if (token[0] == '.') { // Direct Byte
		if (isdigit(token[1])) {
			sscanf(token+1, "%hi", &result);
		} else {
			// Assume character
			// TODO: No Assume
			result = 0x0000;
			sscanf(token+2, "%c", (char *) &result);
		}
		out_add(result);
	}

	else if (token[strlen(token)-1] == ':') {	// Label Definitions

		Label_t *n_label = malloc(sizeof(Label_t));	// Create label entry
		n_label->name = calloc(strlen(token), sizeof(char));
		strncpy(n_label->name, token, strlen(token)-1);
		n_label->addr = ll_len(output); 	// Address is how many insts before this

		ll_add(label_def, n_label);		// Add it to the label list
	} else {
		printf(" >> %s <<\n", token);
		syntax_error("Unknown token");
	}

	free(token);

	return 0;
}

uint8_t expect_reg() {
	Parse_t *r0 = next_num_or_label();
	if (r0->type != REG) {
		printf("%d\n", r0->type);
		syntax_error("Register Expected");
	}
	uint8_t res = r0->value.number;
	free(r0);
	return res; 	// r0
}

// Get Single Register
uint8_t get_reg(char *token) {
	uint8_t result = 0x00;

	if (strlen(token) != 3 || token[0] != '%') {
		fprintf(stderr, " >> Expected register, got %s <<\n", token);
		syntax_error("Unknown Register");
	}
	switch (token[1]) {
		case 'R':
			break;
		case 'S':
			result |= 0x08;
			break;
		default:
			fprintf(stderr, " >> %s <<\n", token);
			syntax_error("Unknown Register");
			break;
	}
	switch (token[2]) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			result |= token[2] - '0';
			break;
		default:
			fprintf(stderr, " >> %s <<\n", token);
			syntax_error("Unknown Register");
			break;
	}
	return result;
}

// Add an instruction to the output
void out_add(uint16_t in) {
	uint16_t *inst = malloc(sizeof(uint16_t));
	*inst = in;
	ll_add(output, inst);
}

// Get the next number or label
Parse_t *next_num_or_label() {
	Parse_t *result = malloc(sizeof(Parse_t));

	char *token = next_token(); // Freed at end of func

	if (token[0] == '#') {
		// Referencing an immidate
		result->type = NUMBER;
		sscanf(token+1, "%hi", &result->value.number);
	} else if (token[0] == '$') {
		// Referencing a label
		result->type = LABEL;

		// Malloc so that we can free token
		result->value.name = calloc(strlen(token+1)+1, sizeof(char));
		strcpy(result->value.name, token+1);
	} else if (token[0] == '%') {
		// Referencing a register
		result->type = REG;
		result->value.number = get_reg(token);
	} else {
		syntax_error("Unknown reference!");
	}

	free(token);

	return result;
}

// Add a label reference
void add_label_ref(uint16_t addr, char *name, uint16_t mask, uint8_t shift) {
	Label_t *n_label = malloc(sizeof(Label_t));	// Create label entry

	char *n_name = calloc(strlen(name) + 1, sizeof(char)); // Copy of string for convience
	strcpy(n_name, name);

	n_label->name = n_name;
	n_label->addr = addr;
	n_label->mask = mask;
	n_label->shft = shift;

	ll_add(label_ref, n_label);		// Add it to the label list
}

// Tokenize all lines of the input file
char *delim = " \t\n\r,";
void tokenize(ll_t *result, FILE *infile) {
	// Create list for tokens
	char *line = NULL;
	char *token = NULL;
	size_t len;

	// Process One Line
	while (getline(&line, &len, infile) != -1) {
		token = strtok(line, delim);

		// For each token in the line
		while (token != NULL) {
			// Ignore comments
			if (strcmp(token, "--") == 0) {
				break;
			}

			// Copy over string for memory sanity
			char *new_t = malloc(strlen(token) + 1);
			strcpy(new_t, token);
			ll_add_iter(result, new_t);

			token = strtok(NULL, delim);
		}

		free(line);
		line = NULL;
	}

	free(line);
	ll_iter_rewind(result);
}

// Get the next token for use
char *next_token() {
	char *result = ll_iter_next(tokens);
	ll_remove_iter(tokens);
	return result;
}

// Free all memory used by program
void free_memory() {
	del_ll(tokens);

	ll_iter_rewind(label_def);
	Label_t *item = ll_iter_next(label_def);

	while (item != NULL) {
		free(item->name);
		free(item);
		item = ll_iter_next(label_def);
	}

	// Free list
	del_ll(label_def);

	ll_iter_rewind(label_ref);
	item = ll_iter_next(label_ref);

	while (item != NULL) {
		free(item->name);
		free(item);
		item = ll_iter_next(label_ref);
	}

	// Free list
	del_ll(label_ref);

	free_ll(output);
	free(out_full);
}

// Fully free a linked list, including it's items
void free_ll(ll_t *list) {
	// Free all allocated items
	ll_iter_rewind(list);
	void *item = ll_iter_next(list);

	while (item != NULL) {
		free(item);
		item = ll_iter_next(list);
	}

	// Free list
	del_ll(list);
}

// Worst error reporting ever :)
// TODO: Not have the worst error reporting ever
void syntax_error(char *error) {
	fprintf(stderr, "Assembly Error: %s\n", error);
	exit(1);
}