#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "assembler.h"
#include "linked_list.h"

linked_list_t *tokens;
linked_list_t *label_def;
linked_list_t *label_ref;
linked_list_t *output;
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
	tokens = tokenize(infile);
	fclose(infile);

	// Initialize linked lists
	label_def = new_linked_list();
	label_ref = new_linked_list();
	output = new_linked_list();

	// Create unlinked instruction stream
	int done;
	do {
		done = proc_instr();
	} while (!done);

	// Create final array of instructions
	out_full = calloc(linked_list_len(output), sizeof(uint16_t));

	// Rewind iterators
	linked_list_iter_rewind(output);
	linked_list_iter_rewind(label_ref);

	// Get first elements
	uint16_t *inst = (uint16_t *) linked_list_iter_next(output);
	Label_t *c_ref = linked_list_iter_next(label_ref);

	// For every instruction, linking if needed
	int i = 0;
	Label_t *c_def;
	while (inst != NULL) {
		if (c_ref != NULL && c_ref->addr == i) {
			// Found Reference

			// Find associated definition
			linked_list_iter_rewind(label_def);
			c_def = linked_list_iter_next(label_def);
			while (strcmp(c_ref->name, c_def->name) != 0) {
				c_def = linked_list_iter_next(label_def);

				// Ran off the end, missing definition
				if (c_def == NULL) {
					printf(" >> %s <<\n", c_ref->name);
					syntax_error("Undefined Label");
				}
			}

			// Update associated address
			*inst |= (c_def->addr & c_ref->mask) >> c_ref->shft;

			c_ref = linked_list_iter_next(label_ref);
		}

		// Move instruction to final list
		out_full[i++] = *inst;

		// Get next instruction
		inst = (uint16_t *) linked_list_iter_next(output);
	}

	// Write the final program to a file
	FILE *ofile = fopen(argv[2], "wb");
	fwrite(out_full, 2, linked_list_len(output), ofile);
	fclose(ofile);

	free_memory();

	printf("Assembly Successful\n");

	exit(0);
}

// Generate the instruction for each line of the input
int proc_instr() {
	uint16_t result;
	char *token = next_token();
	if (token == NULL) {
		return 1; // EOF, expected exit
	}

	// "Real" Instructions
	if (strcmp(token, "ADD") == 0) {		// ADD r1, r2, r0
		result = 0x0000;
		result |= get_reg() << 4;	// r1
		result |= get_reg();		// r2
		result |= get_reg() << 8;	// r0
		out_add(result);
	} else if (strcmp(token, "SUB") == 0) {	// SUB r1, r2, r0
		result = 0x1000;
		result |= get_reg() << 4;	// r1
		result |= get_reg();		// r2
		result |= get_reg() << 8;	// r0
		out_add(result);
	} else if (strcmp(token, "LIL") == 0) {	// LIL i,  r0
		result = 0x2000;

		Parse_t *t = next_num_or_label();
		if (t->type = LABEL) {
			add_label_ref(linked_list_len(output), t->value.name, 0xFF, 0);
		} else {
			result |= t->value.number & 0xFF;
		}
		free(t);

		result |= get_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "LIH") == 0) {	// LIH i,  r0
		result = 0x3000;

		Parse_t *t = next_num_or_label();
		if (t->type = LABEL) {
			add_label_ref(linked_list_len(output), t->value.name, 0xFF, 8);
		} else {
			result |= (t->value.number >> 8) & 0xFF;
		}
		free(t);

		result |= get_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "STO") == 0) { // STO r0, r1
		result = 0x4000;
		result |= get_reg() << 8;
		result |= get_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "LOA") == 0) { // LOA r1, r0
		result = 0x5000;
		result |= get_reg() << 4;
		result |= get_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "MOV") == 0) { // MOV r1, r0
		result = 0x6000;
		result |= get_reg() << 4;
		result |= get_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "ADI") == 0) { // ADI i,  r0
		result = 0x7000;

		Parse_t *t = next_num_or_label();
		if (t->type = LABEL) {
			add_label_ref(linked_list_len(output), t->value.name, 0xFF, 0);
		} else {
			result |= t->value.number & 0xFF;
		}
		free(t);

		result |= get_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "SKL") == 0) { // SKL r0, r1
		result = 0x8000;
		result |= get_reg() << 8;
		result |= get_reg() << 4;
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
		if (t->type = LABEL) {
			add_label_ref(linked_list_len(output), t->value.name, 0xFF, 0);
		} else {
			result |= t->value.number & 0xFF;
		}
		free(t);

		result |= get_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "IOW") == 0) { // IOW r0, i
		result = 0xC000;
		result |= get_reg() << 8;

		Parse_t *t = next_num_or_label();
		if (t->type = LABEL) {
			add_label_ref(linked_list_len(output), t->value.name, 0xFF, 0);
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
		
		result |= get_reg() << 8;

		if (t->type == LABEL) {
			add_label_ref(linked_list_len(output), t->value.name, 0xFF, 0);
			add_label_ref(linked_list_len(output)+1, t->value.name, 0xFF, 8);
			out_add(result);
			out_add(result | 0x1000);
		} else {
			out_add(result | (t->value.number & 0xFF));
			out_add(result | 0x1000 | ((t->value.number & 0xFF) >> 8));
		}
		free(t);
	} else if (strcmp(token, "JMP") == 0) { // JMP r1
		result = 0x6700;
		result |= get_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "JSR") == 0) { // JSR r1
		out_add(0x6570); // MOV PC, R5
		out_add(0x7505); // ADI 5,  R5
		out_add(0x4560); // STO R5, SP
		out_add(0x7601); // ADI 1,  SP

		// MOV r1, PC
		result = 0x6700;
		result |= get_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "PSH") == 0) { // PSH r0 
		result = 0x4060;

		result |= get_reg() << 8;

		out_add(result); // STO r0, SP
		out_add(0x7601); // ADI 1,  SP
	} else if (strcmp(token, "POP") == 0) { // POP r0 
		out_add(0x76FF); // ADI -1, SP

		result = 0x5060;

		result |= get_reg() << 8;

		out_add(result); // LOA SP, r0
		
	} else if (strcmp(token, "JGT") == 0) { // JGT r0, r1, r2
		// If r0 > r1, goto r2 
		
		// SKL r0, r1
		result = 0x8000;
		result |= get_reg() << 8;
		result |= get_reg() << 4;
		out_add(result);
		
		// MOV r2, PC
		result = 0x6700;
		result |= get_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "JGT") == 0) { // RET
		// POP return address and jump
		out_add(0x76FF); // ADI		-1, 	SP
		out_add(0x5760); // LOA 	SP,		PC
	}

	else if (token[strlen(token)-1] == ':') {	// Label Definitions
		token[strlen(token)-1] = '\0';				// Chop off colon

		Label_t *n_label = malloc(sizeof(Label_t));	// Create label entry
		n_label->name = token;
		n_label->addr = linked_list_len(output); 	// Address is how many insts before this

		linked_list_add(label_def, n_label);		// Add it to the label list
	} else {
		printf(" >> %s <<\n", token);
		syntax_error("Unknown token");
	}
	return 0;
}

// Get Single Register
uint8_t get_reg() {
	uint8_t result = 0x00;
	char *token = next_token();
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
	linked_list_add(output, inst);
}

// Get the next number or label
Parse_t *next_num_or_label() {
	Parse_t *result = malloc(sizeof(Parse_t));

	char *token = next_token();
	if (isdigit(token[0])) {
		result->type = NUMBER;
		sscanf(token, "%hi", &result->value.number);
	} else {
		// Referencing a label
		result->type = LABEL;
		result->value.name = token;
	}

	return result;
}

// Add a label reference
void add_label_ref(uint16_t addr, char *name, uint16_t mask, uint8_t shift) {
	Label_t *n_label = malloc(sizeof(Label_t));	// Create label entry

	char *n_name = calloc(strlen(name), sizeof(char)); // Copy of string for convience
	strcpy(n_name, name);

	n_label->name = n_name;
	n_label->addr = addr;
	n_label->mask = mask;
	n_label->shft = shift;

	linked_list_add(label_ref, n_label);		// Add it to the label list
}

// Tokenize all lines of the input file
char *delim = " \t\n\r,";
linked_list_t *tokenize(FILE *infile) {
	// Create list for tokens
	linked_list_t *result = new_linked_list();
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
			linked_list_add(result, new_t);

			token = strtok(NULL, delim);
		}

		free(line);
		line = NULL;
	}

	free(line);
	
	linked_list_iter_rewind(result);
	return result;
}

// Get the next token for use
char *next_token() {
	return linked_list_iter_next(tokens);
}

// Free all memory used by program
void free_memory() {
	free_ll(tokens);
	free_ll(label_def);
	free_ll(label_ref);
	free_ll(output);
	free(out_full);
}

// Fully free a linked list, including it's items
void free_ll(linked_list_t *list) {
	// Free all allocated items
	linked_list_iter_rewind(list);
	void *item = linked_list_iter_next(list);

	while (item != NULL) {
		free(item);
		item = linked_list_iter_next(list);
	}

	// Free list
	del_linked_list(list);
}

// Worst error reporting ever :)
// TODO: Not have the worst error reporting ever
void syntax_error(char *error) {
	fprintf(stderr, "Assembly Error: %s\n", error);
	exit(1);
}