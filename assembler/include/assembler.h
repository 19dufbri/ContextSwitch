#include <stdint.h>
#include "linked_list.h"

#ifndef ASSEM_H
#define ASSEM_H

typedef struct {
	char *name;
	uint16_t addr;
	uint16_t mask;
	uint8_t shft;
} Label_t;

typedef struct {
	enum {
		LABEL,
		NUMBER
	} type;
	union {
		char *name;
		uint16_t number;
	} value;
} Parse_t;

int proc_instr();
uint8_t get_reg();
char *next_token();
void syntax_error(char *error);
linked_list_t *tokenize(FILE *infile);
void out_add(uint16_t in);
int8_t next_byte();
int16_t next_short();
void free_ll(linked_list_t *list);
void free_memory();
Parse_t *next_num_or_label();
void add_label_ref(uint16_t addr, char *name, uint16_t mask, uint8_t shift);

#endif