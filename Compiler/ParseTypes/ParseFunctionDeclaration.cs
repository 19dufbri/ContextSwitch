using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public class ParseFunctionDeclaration : Parse
    {
        private readonly string _datatype;
        private readonly string _name;
        private readonly List<ParseVariableDeclaration> _arguments;
        private readonly ParseBlock _block;

        public ParseFunctionDeclaration(List<string> tokens)
        {
            _datatype = tokens.First();
            tokens.RemoveAt(0);
            _name = tokens.First();
            tokens.RemoveAt(0);

            _arguments = GetFunctionArgs(tokens);

            if (tokens.First() == "{")
            {
                // Definition too
                _block = new ParseBlock(tokens);
            }
            else if (tokens.First() == ";")
            {
                tokens.RemoveAt(0);
            }
            else
            {
                ParseProgram.ReportError("Expected block or semicolon");
            }
        }

        private static List<ParseVariableDeclaration> GetFunctionArgs(List<string> tokens)
        {
            var result = new List<ParseVariableDeclaration>();
            
            if (tokens.First() != "(")
                ParseProgram.ReportError("Expected argument list");
            tokens.RemoveAt(0);
            
            var done = tokens.First() == ")";
            while (!done)
            {
                result.Add(new ParseVariableDeclaration(tokens));
                if (tokens.First() == ",")
                    tokens.RemoveAt(0);
                else
                    done = true;
            }
            
            if (tokens.First() != ")")
                ParseProgram.ReportError("Expected end of argument list");
            tokens.RemoveAt(0);

            return result;
        }

        public override List<string> EmitInstructions()
        {
            List<string> result = new();
            
            result.Add("#FUNCTION " + _datatype + " " + _name);
            foreach (var argument in _arguments)
            {
                result.AddRange(argument.EmitInstructions());
            }
            result.Add("#ENDPARAMS");
            
            // No block, just definition
            if (_block == null) return result;
            
            // Function definition
            result.Add("#FUNCTIONDEF");
            result.Add("Routine" + _name + ":");
            result.AddRange(_block.EmitInstructions());

            return result;
        }
    }
}