using System;
using System.Collections.Generic;

namespace Assembler
{
    public class Linker
    {
        public static void Link(List<ushort> instructions, List<Label> definitions, List<Label> references)
        {
            // For every reference
            foreach (var reference in references)
            {
                // Find associated definition
                var definition = definitions.Find(l => l.Name.Equals(reference.Name));
                if (definition == null)
                {
                    Console.WriteLine(" >> " + reference.Name + " <<\n");
                    Program.syntax_error("Undefined Label");
                    return;
                }

                var offset = (ushort) (definition.Addr - reference.Addr - 3);
                instructions[reference.Addr] = (ushort) (instructions[reference.Addr] | offset & 0xFF);
                instructions[reference.Addr+1] = (ushort) (instructions[reference.Addr+1] | (offset >> 8) & 0xFF);
            }
        }
    }
}