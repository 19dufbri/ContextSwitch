using System;
using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public abstract class Parse
    {
        // To identify different jump destinations
        protected string Guid { get; } = System.Guid.NewGuid().ToByteArray().Aggregate("", (s, b) => s + (char) (b % 26 + 'A'));
        public abstract List<string> EmitInstructions();
    }
}