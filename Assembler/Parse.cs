using System;

namespace Assembler
{
    public class Parse
    {
        public enum ParseType
        {
            Label,
            Number,
            Reg
        }

        public Parse(string name)
        {
            Type = ParseType.Label;
            Name = name;
        }

        public Parse(ParseType type, ushort number)
        {
            if (type != ParseType.Reg && type != ParseType.Number)
                throw new ArgumentException("Parse type must be REG or NUMBER when called with a ushort", nameof(type));
            Type = type;
            Number = number;
        }

        public ParseType Type { get; }
        public string Name { get; }
        public ushort Number { get; }
    }
}