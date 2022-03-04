using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public class ParseVariableAssignment : ParseStatement
    {
        private readonly string _name;
        private readonly ParseEqualityExpression _definition;
        public ParseVariableAssignment(List<string> tokens)
        {
            _name = tokens.First();
            tokens.RemoveAt(0);
            
            if (tokens.First() != "=")
                ParseProgram.ReportError("Expected assignment");
            tokens.RemoveAt(0);
            
            _definition = new ParseEqualityExpression(tokens);

            if (tokens.First() != ";")
                ParseProgram.ReportError("Expected semicolon");
        }

        public override List<string> EmitInstructions()
        {
            List<string> result = new();
            
            result.AddRange(_definition.EmitInstructions());
            result.Add("STI %R0, $" + _name);
            
            return result;
        }
    }
}