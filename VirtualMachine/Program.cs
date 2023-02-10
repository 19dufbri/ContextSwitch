using System;

namespace VirtualMachine {
	public static class Program
	{
		[STAThread]
		private static void Main(string[] args) {
			if (args.Length < 1) {
				Console.Error.WriteLine("Too few arguments!");
				Console.Error.WriteLine("Usage: " + Environment.CommandLine + " <binary_file>");
				return;
			}

			var core = new VirtualMachine();

			// Read in program
			var memory = System.IO.File.ReadAllBytes(args[0]);
			if (memory.Length % 2 != 0 || memory.Length > 0x20000)
				return;
			for (ushort addr = 0; addr < memory.Length / 2; addr++)
			{
				var data = (ushort) ((memory[addr*2 + 1] << 8) | memory[addr*2]);
				core.SetMemory(addr, data);
			}

			// Run forever 
			while (true) {
				if (!core.DoCycle())
					break;
			}
		}
	}
}