# Machine Reference


## Architecture
This is a very simple 16-bit RISC architecture partially based on Chip-8 and MIPS utilizing 16 registers labeled R0-R7 and S0-S7. The processor supports two different privledge levels, IO operations, and interupts.

### Privledge Levels
The CPU supports two levels of execution: Machine and User mode. Machine mode has access to all registers, including the system saved registers S0-S7. The CPU always starts in machine mode as a reboot is marked as an interupt. User mode is only able to access the regular R0-R7. 

Switching between modes is triggered by an interupt, either from an external IO event, a timer, or a user process. When switching to and from machine mode the S and R series of registers are swapped. Execution of interupt code starts at `0x0000` and continues until the exit ISR opcode is reached.

### IO Space
The processor has a limited IO space whereby the user can read or write to any of 0x100 IO locations. The reads and writes are all 16-bits. IO devices may also register 0x10 interupts per IO location as described in the <i>Interupts</i> section.

### Interupts
The CPU can register interupts one at a time in the form of a limited queue. Each interupt will be serviced in the order that it arrives at the controller. When an interupt is registered, the CPU enters machine mode with the interupt source being loaded into R0. The currently defined values for R0 are listed below.

| Source | R0 Value | Description                     |
|--------|----------|---------------------------------|
| Reboot | `0x0000` | When the processor starts       |
| User   | `0x0001` | User process calls `INT`        |
| IO     | `0x1YXX` | IO device `XX` has interupt `Y` |

When there is an IO based interupt, the interupt will contain what device is calling the interupt marked by XX and a device specific interupt number Y. What these numbers mean and how they're handled is managed on a device-by-device basis.

### ISA Reference
The virtual machine uses a RISC based instruction set with a maximum of 16 different instructions. The 16 instructions have no specific method of calling a function, jumping, or using a stack; these operations are handled by a series of pseudo-instructions implemented by the assembler.

Let rX be a reference to a register from the instruction and RX/SX be a specific register

Instruction Format:
```
XXXX    XXXX    XXXX    XXXX
Opcode  r0      r1      r2
```

| Opcode |C Equivalent     | Description                                                |
|--------|-----------------|------------------------------------------------------------|
| `0x0`  |`r0 = r1 + r2`   | Add two registers                                          |
| `0x1`  |`r0 = r1 - r2`   | Subtract two registers                                     |
| `0x2`  |`r0[7..0] = i`   | Load 8-bits into the low side of a register                |
| `0x3`  |`r0[15..8] = i`  | Load 8-bits into the high side of a register               |
| `0x4`  |`mem[r1] = r0`   | Store register at another register's location              |
| `0x5`  |`r0 = mem[r1]`   | Load register from another's location                      |
| `0x6`  |`r1 = r0`        | Copy one register to another                               |
| `0x7`  |`r0 += i`        | Add an signed 8-bit offset to a register                   |
| `0x8`  |`r0 < r1 ? PC++` | Skip next instruction if one register is less than another |
| `0x9`  |Call ISR         | Call an interupt                                           |
| `0xA`  |Exit ISR         | Return from an interupt                                    |
| `0xB`  |`r0 = io[i]`     | Read from an IO location                                   |
| `0xC`  |`io[i] = r0`     | Write to an IO location                                    |
| `0xD`  |Undefined        | Undefined                                                  |
| `0xE`  |Undefined        | Undefined                                                  |
| `0xF`  |Undefined        | Undefined                                                  |

## Assembler
Since the CPU is so limited in its instruction set, the assembler has several pseudo-instructions in addition to the normal instructions of the processor. These add functionality for jumping, stack operations, calling, and more. 

A list of all instructions are listed below

| Mnemonic         | Cycles | C Equivalent                   | Description                        |
|------------------|:------:|--------------------------------|------------------------------------|
| `ADD r1, r2, r0` |    1   | `r0 = r1 + r2`                 | Add two numbers                    |
| `SUB r1, r2, r0` |    1   | `r0 = r1 - r2`                 | Subtract two numbers               |
| `LIL i, r0`      |    1   | `r0[7..0] = i`                 | Load 8-low bits                    |
| `LIH i, r0`      |    1   | `r0[15..8] = i`                | Load 8-high bits                   |
| `STO r0, r1`     |    1   | `mem[r1] = r0`                 | Store `r0` at `mem[r1]`            |
| `LOA r1, r0`     |    1   | `r0 = mem[r1]`                 | Load `r0` from `mem[r1]`           |
| `MOV r1, r0`     |    1   | `r0 = r1`                      | Copy `r1` to `r0`                  |
| `ADI i, r0`      |    1   | `r0 += i`                      | Add an immediate                   |
| `SKL r0, r1`     |    1   | `r0 < r1 ? continue : PC++`    | Skip next instruction if less than |
| `INT`            |    1   | Call ISR                       | Call a software interupt           |
| `EIN`            |    1   | End ISR                        | End an ISR                         |
| `IOR i, r0`      |    1   | `r0 = io[i]`                   | Read from an IO location           |
| `IOW r0, i`      |    1   | `io[i] = r0`                   | Write to an IO location            |
| `LIR ii, r0`     |    2   | `r0 = ii`                      | Load 16-bit immediate              |
| `JMP r0`         |    1   | `goto r0`                      | Goto location in register          |
| `JSR r0`         |    5   | `r0()`                         | Call a subroutine                  |
| `PSH r0`         |    2   | `[SP++] = r0`                  | Push `r0` onto the stack           |
| `POP r0`         |    2   | `r0 = [--SP]`                  | Pop `r0` from the stack            |
| `JGT r0, r1, r2` |   4?   | `r0 < r1 ? goto r2 : continue` | Jump if greater than               |