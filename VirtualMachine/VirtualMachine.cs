using System;

namespace VirtualMachine
{
    public class VirtualMachine
    {
	    private enum MachineMode
	    {
		    Machine,
		    User
	    }
	    
	    private readonly ushort[] _regs = new ushort[0x10];
        private readonly ushort[] _memory = new ushort[0x10000];
        private MachineMode _mode = MachineMode.Machine;
        
        private const int Pc = 0x7;

        private readonly IPeripheral[] _peripherals;

        public VirtualMachine()
        {
	        _peripherals = new IPeripheral[0x100];
	        _peripherals[0x01] = _peripherals[0x02] = new TimerPeripheral();
        }

        public void SetMemory(ushort address, ushort data)
        {
	        _memory[address] = data;
        }

        private void DoPeripherals()
        {
	        var found = false;
	        foreach (var t in _peripherals)
	        {
		        var interrupt = t?.RunCycle();
		        
		        if (found || !interrupt.HasValue)
			        continue;
		        found = true;
		        
		        DoUndoInt();
		        _regs[0x0] = interrupt.Value;
	        }
        }

        public bool DoCycle() {
	        // Do peripherals such as timer, input, and output
			DoPeripherals();

			// Run one instruction
			return do_opcode();
		}

        private void DoUndoInt()
        {
	        // Swap register
	        for (var i = 0; i < 0x8; i++)
		        (_regs[i], _regs[i + 0x8]) = (_regs[i + 0x8], _regs[i]);
		    // Switch modes
		    _mode = _mode == MachineMode.Machine ? MachineMode.User : MachineMode.Machine;
        }

		private bool do_opcode() {
			// Opcode to run
			var opcode = _memory[_regs[Pc]++];

			// Register Selectors
			var r0 = (ushort) ((opcode >> 8) & 0x0F);
			var r1 = (ushort) ((opcode >> 4) & 0x0F);
			var r2 = (ushort) (opcode & 0x0F);

			// Protect S0-S7
			if (_mode == MachineMode.User) {
				r0 &= 0x7;
				r1 &= 0x7;
				r2 &= 0x7;
			}

			// Opcode decode
			switch (opcode >> 12) {
				case 0x0: // ADD - Addition
					_regs[r0] = (ushort) (_regs[r1] + _regs[r2]);
					break;
				case 0x1: // Unused
					_regs[r0] = (ushort) (_regs[r1] - _regs[r2]);
					break;
				case 0x2: // LIL - Load Immediate Low
					_regs[r0] &= 0xFF00;
					_regs[r0] = (ushort) (_regs[r0] | opcode & 0xFF);
					break;
				case 0x3: // LIH - Load Immediate High
					_regs[r0] &= 0x00FF;
					_regs[r0] = (ushort) (_regs[r0] | (opcode & 0xFF) << 8);
					break;
				case 0x4: // STO
					_memory[_regs[r1]] = _regs[r0];
					break;
				case 0x5: // LOA
					_regs[r0] = _memory[_regs[r1]];
					break;
				case 0x6: // MOV
					_regs[r0] = _regs[r1];
					break;
				case 0x7: // ADI 
					_regs[r0] = (ushort) (_regs[r0] + (sbyte) (opcode & 0xFF));
					break;
				case 0x8: // SKL
					if ((short) _regs[r0] < (short) _regs[r1])
						_regs[Pc]++;
					break;
				case 0x9: // INT
				    if (_mode == MachineMode.User)
					    _regs[0x8] = 0x0001;
					DoUndoInt();
					break;
				case 0xA: // Unused
					break;
				case 0xB: // IOR
				{
					var peripheral = _peripherals[opcode & 0xFF];
					_regs[r0] = (peripheral?.DoRead((byte) (opcode & 0xFF))).GetValueOrDefault(0x0000);
					break;
				}
				case 0xC: // IOW
				{
					var peripheral = _peripherals[opcode & 0xFF];
					peripheral?.DoWrite((byte) (opcode & 0xFF), _regs[r0]);
					break;
				}
				case 0xD: // AND
					_regs[r0] = (ushort) (_regs[r1] & _regs[r2]);
					break; 
				case 0xE: // XOR
					_regs[r0] = (ushort) (_regs[r1] ^ _regs[r2]);
					break;
				case 0xF: // BRK
					// Debugging statement
					Console.WriteLine("Mode: " + _mode);
					for (var i = 0; i < 0x10; i++) 
						Console.WriteLine((i < 0x8 ? "R" : "S") + i % 0x8 + ": " + _regs[i]);
					Console.WriteLine("\n");
					break;
				default:
					return false;
			}

			return true;
		}
    }
}