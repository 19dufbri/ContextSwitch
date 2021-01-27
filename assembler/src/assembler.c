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
		if (c_ref->addr == i) {
			// Low Byte

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
			*inst |= c_def->addr & 0xFF;

		} else if (c_ref->addr == i+1) {
			// Low Byte
			*inst |= c_def->addr >> 8;
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

	exit(0);
}

char *keywords[] = {
						"ADD",
						"SUB",
						"JMP",
						"JSR",
						"LDI",
						"LIL",
						"LIH",
						"SIE",
						"SNE",
						"REI",
						"MOV",
						"STO",
						"LOA",
						"RET"
					};

// Generate the instruction for each line of the input
int proc_instr() {
	uint16_t result;
	char *token = next_token();
	if (token == NULL) {
		return 1; // EOF, expected exit
	}
	if (strcmp(token, "ADD") == 0) {		// ADD r0 = r1 + r2
		result = 0x0000;
		result |= get_all_regs();
		out_add(result);
	} else if (strcmp(token, "SUB") == 0) {	// SUB r0 = r1 - r2
		result = 0x1000;
		result |= get_all_regs();
		out_add(result);
	} else if (strcmp(token, "LIL") == 0) {	// Load Imm Low r0[7 .. 0] = imm
		result = 0x2000;
		result |= next_byte();
		result |= get_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "LIH") == 0) {	// Load Imm Low r0[15 .. 7] = imm
		result = 0x3000;
		result |= next_byte();
		result |= get_reg() << 8;
		out_add(result);
	} else if (strcmp(token, "LDI") == 0) { // Psuedo instruction, LIL & LIH
		uint16_t imm = next_short();
		uint16_t reg = get_reg() << 8;
		
		result = 0x2000;
		result |= reg;
		result |= imm & 0xFF;
		out_add(result);

		result = 0x3000;
		result |= reg;
		result |= imm >> 8;
		out_add(result);
	} else if (strcmp(token, "JPR") == 0) { // JumP Register
		result = 0x8000;
		result |= get_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "JSR") == 0) { // Jump Subroutine Immidiate
		result = 0x8100;
		result |= get_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "SEQ") == 0) { // Skip EQual
		result = 0x9000;
		result |= get_reg() << 4;
		result |= get_reg();
		out_add(result);
	} else if (strcmp(token, "SNE") == 0) { // Skip if Not Equal
		result = 0x9100;
		result |= get_reg() << 4;
		result |= get_reg();
		out_add(result);
	} else if (strcmp(token, "REI") == 0) { // REturn from Interupt
		result = 0xA100;
		out_add(result);
	} else if (strcmp(token, "MOV") == 0) { // MOVe r0 = r1
		result = 0x7000;
		result |= get_reg() << 8;
		result |= get_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "STR") == 0) { // STore at Register
		result = 0x4000;
		result |= get_reg() << 4;
		result |= get_reg();
		out_add(result);
	} else if (strcmp(token, "LDR") == 0) { // LoaD at Register
		result = 0x4100;
		result |= get_reg() << 4;
		result |= get_reg();
		out_add(result);
	} else if (strcmp(token, "RET") == 0) { // RETurn from subroutine
		result = 0xA000;
		out_add(result);
	} else if (strcmp(token, "INT") == 0) { // INTerupt the processor
		result = 0xD000;
		out_add(result);
	} else if (strcmp(token, "RDK") == 0) { // ReaD the Keyboard
		result = 0xE000;
		result |= get_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "WRS") == 0) { // WRite to the Screen
		result = 0xE100;
		result |= get_reg() << 4;
		out_add(result);
	} else if (strcmp(token, "HLT") == 0) { // HaLT the processor
		result = 0xF000;
		out_add(result);
	} else if (token[strlen(token)-1] == ':') {	// Label Definitions
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

// Get all the registers for ALU instructions
uint16_t get_all_regs() {
	uint16_t result = 0x0000;
	result |= get_reg() << 8;
	result |= get_reg() << 4;
	result |= get_reg();
	return result;
}

// Get Single Register
uint8_t get_reg() {
	uint8_t result = 0x00;
	char *token = next_token();
	switch (token[0]) {
		case 'R':
			break;
		case 'S':
			result |= 0x08;
			break;
		default:
			syntax_error("Unknown Register");
			break;
	}
	switch (token[1]) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			result |= token[1] - '0';
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			result |= token[1] - 'A' + 0x0A;
			break;
		default:
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

// Get the 8-bit value of the next token
int8_t next_byte() {
	int result;
	char *token = next_token();
	if (isdigit(token[0])) {
		sscanf(token, "%i", &result);
		return result & 0xFF;
	} else {
		// Referencing a label
		Label_t *n_label = malloc(sizeof(Label_t));	// Create label entry
		n_label->name = token;
		n_label->addr = linked_list_len(output); 	// Address is how many insts before this

		linked_list_add(label_ref, n_label);		// Add it to the label list
		return 0;
	}
}

// Get the 16-bit value of the next token
int16_t next_short() {
	int result;
	char *token = next_token();
	if (isdigit(token[0])) {
		sscanf(token, "%i", &result);
		return result & 0xFFFF;
	} else {
		// Referencing a label
		printf("Found label ref: %s\n", token);
		Label_t *n_label = malloc(sizeof(Label_t));	// Create label entry
		n_label->name = token;
		n_label->addr = linked_list_len(output); 	// Address is how many insts before this

		linked_list_add(label_ref, n_label);		// Add it to the label list
		return 0;
	}
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