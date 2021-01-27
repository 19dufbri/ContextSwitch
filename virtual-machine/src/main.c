typedef struct {
	uint16_t pc, a, b, sp;
	uint16_t *memory;
} SimpleCore;

int main(int argc, char *argv[]) {

}

int do_opcode(SimpleCore *core) {
	uint16_t opcode = core->memory[core->pc++];
	switch (opcode >> 12) {
		case 0x0: // ALU
		case 0x1:
		case 0x3:
			core->pc += core->a;
			break;
		case 0x4:
		case 0x5:
		case 0x6: 
		case 0x7: // Store Register
		case 0x8: // Store Lower Byte
		case 0x9: // Jump
		case 0xA: // Call
		case 0xB: // Return
		case 0xC: // Yeild
		case 0xD: // Write Screen
		case 0xE: // Read Keyboard
		case 0xF: // Exit
	}
}