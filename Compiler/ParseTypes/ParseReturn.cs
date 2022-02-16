using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public class ParseReturn : ParseStatement
    {
        private readonly ParseRValue _returnValue;
        public ParseReturn(List<string> tokens)
        {
            var token = tokens.First();
            if (token != "return")
                ParseProgram.ReportError("Expected `return`");
            tokens.RemoveAt(0);

            if (tokens.First() != ";")
                _returnValue = new ParseRValue(tokens);
        }

        public override List<string> EmitInstructions()
        {
            List<string> result = new();

            if (_returnValue != null)
                result.AddRange(_returnValue.EmitInstructions());
            result.Add("RET");
            
            return result;
        }
    }
}