using System;
using System.Collections.Generic;

namespace Assembler
{
    public class Linker
    {
        public static void Link(List<ushort> instructions, List<Label> definitions, List<Label> references)
        {
            for (var i = 0; i < instructions.Count; i++)
            {
                var inst = instructions[i];

                foreach (var reference in references)
                {
                    if (reference.Addr != i) continue;

                    // Find associated definition
                    var label = definitions.Find(l => l.Name.Equals(reference.Name));
                    if (label == null)
                    {
                        Console.WriteLine(" >> " + reference.Name + " <<\n");
                        Program.syntax_error("Undefined Label");
                        return;
                    }

                    inst |= (ushort) ((label.Addr >> reference.Shift) & reference.Mask);
                }

                instructions[i] = inst;
            }
        }
    }
}