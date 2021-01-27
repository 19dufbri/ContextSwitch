#include <stdint.h>
#include "linked_list.h"

#ifndef ASSEM_H
#define ASSEM_H

typedef struct {
	char *name;
	uint16_t addr;
} Label_t;

int proc_instr();
uint16_t get_all_regs();
uint8_t get_reg();
char *next_token();
void syntax_error(char *error);
linked_list_t *tokenize(FILE *infile);
void out_add(uint16_t in);
int8_t next_byte();
int16_t next_short();
void free_ll(linked_list_t *list);
void free_memory();

#endif