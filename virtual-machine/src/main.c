#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define PC	0x7

typedef enum {
	NORMAL,
	EXIT_INT,
	NEW_INT
} SimpleCoreState;

typedef struct {
	uint16_t regs[0x100];
	uint16_t *memory;
	bool machine_mode;
	SimpleCoreState interupt;
} SimpleCore;

int do_cycle(SimpleCore *core);
int do_opcode(SimpleCore *core);

int main(int argc, char *argv[]) {
	if (argc < 2) {
		return 1;
	}

	// Allocate memory for processor
	SimpleCore *core = calloc(sizeof(SimpleCore), 1);
	core->memory = calloc(sizeof(uint16_t), 0x10000);

	// Set inital state
	core->interupt = NORMAL;
	core->machine_mode = true;

	// Read in program
	FILE *infile = fopen(argv[1], "r");
	int remain = 0x10000;
	while (!feof(infile)) {
		remain -= fread(core->memory, 2, remain, infile);
	}
	fclose(infile);

	// Run forever 
	while (true) {
		do_cycle(core);
	}
}

int do_cycle(SimpleCore *core) {
	static int timer = 0x100;

	// If timer is zero, make interupt
	if (timer == 0) {
		timer = 0x100;
		core->interupt = NEW_INT;
	}

	// Handle Interupt Changes
	if (core->interupt == NEW_INT) {
		for (int i = 0; i < 0x10; i++) {
			uint16_t temp = core->regs[i];
			core->regs[i] = core->regs[i + 0x10];
			core->regs[i + 0x10] = temp;
		}
		core->machine_mode = true;
		core->interupt = NORMAL;
	} else if (core->interupt == EXIT_INT) {
		for (int i = 0; i < 0x10; i++) {
			uint16_t temp = core->regs[i];
			core->regs[i] = core->regs[i + 0x10];
			core->regs[i + 0x10] = temp;
		}
		core->machine_mode = false;
		core->interupt = NORMAL;
	}

	// Decrement timer if in user code
	core->machine_mode ? 0x00 : timer--;

	// Run one instruction
	do_opcode(core);
}

int do_opcode(SimpleCore *core) {
	// Opcode to run
	uint16_t opcode = core->memory[core->regs[PC]++];

	// Register Selectors
	uint8_t r0 = (opcode >> 8) & 0x0F;
	uint8_t r1 = (opcode >> 4) & 0x0F;
	uint8_t r2 = opcode & 0x0F;

	// Protect S0-S7
	if (!core->machine_mode) {
		r0 &= 0x7;
		r1 &= 0x7;
		r2 &= 0x7;
	}

	// Opcode decode
	switch (opcode >> 12) {
		case 0x0: // ADD - ADDition
			core->regs[r0] = core->regs[r1] + core->regs[r2];
			break;
		case 0x1: // SUB - SUBtract
			core->regs[r0] = core->regs[r1] - core->regs[r2];
			break;
		case 0x2: // LIL - Load Immidiate Low
			core->regs[r0] &= 0xFF00;
			core->regs[r0] |= opcode & 0xFF;
			break;
		case 0x3: // LIH - Load Immidiate High
			core->regs[r0] &= 0x00FF;
			core->regs[r0] |= (opcode & 0xFF) >> 8;
			break;
		case 0x4: // STO
			core->memory[core->regs[r1]] = core->regs[r0];
			break;
		case 0x5: // LOA
			core->regs[r0] = core->memory[core->regs[r1]];
			break;
		case 0x6: // MOV
			core->regs[r0] = core->regs[r1];
			break;
		case 0x7: // ADI 
			{
				uint8_t imm = opcode & 0xFF;
				core->regs[r0] += imm;
			}
			break;
		case 0x8: // SKL
			if (core->regs[r0] < core->regs[r1]) {
				core->regs[PC]++;
			}
			break;
		case 0x9: // INT
			core->interupt = NEW_INT;
			core->regs[0] = 0x0001;
			break;
		case 0xA: // EIN
			core->interupt = EXIT_INT;
			break;
		case 0xB: // IOR
			break;
		case 0xC: // IOW
			printf("%c", core->regs[r0] & 0xFF);
			break;
		case 0xD:
		case 0xE:
		case 0xF:
		default:
			exit(1);
	}
}