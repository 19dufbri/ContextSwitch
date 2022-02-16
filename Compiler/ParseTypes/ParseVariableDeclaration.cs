using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public class ParseVariableDeclaration : ParseStatement
    {
        private readonly string _datatype;
        private readonly string _name;
        private readonly ParseRValue _definition;
        
        public ParseVariableDeclaration(List<string> tokens)
        {
            _datatype = tokens.First();
            tokens.RemoveAt(0);
            _name = tokens.First();
            tokens.RemoveAt(0);
            
            // No definition, return
            if (tokens.First() != "=") return;
            
            // Includes definition
            tokens.RemoveAt(0);
            _definition = new ParseRValue(tokens);
        }

        public override List<string> EmitInstructions()
        {
            List<string> result = new();
            
            result.Add("#VARIABLE " + _name);
            
            // No definition, return
            if (_definition == null) return result;
            
            // Add definition
            result.AddRange(_definition.EmitInstructions());
            result.Add("STI %R0, $" + _name);

            return result;
        }
    }
}