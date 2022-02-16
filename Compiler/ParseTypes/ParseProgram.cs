using System;
using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public class ParseProgram : Parse
    {
        private readonly List<ParseFunctionDeclaration> _functions;
        private readonly List<ParseVariableDeclaration> _variables;

        public ParseProgram(List<string> tokens)
        {
            _functions = new List<ParseFunctionDeclaration>();
            _variables = new List<ParseVariableDeclaration>();

            while (tokens.Count > 0)
            {
                if (!tokens[0].All(char.IsLetter) || !tokens[1].All(char.IsLetter))
                    ReportError("Expected variable or function declaration");
                if (tokens[2] == "(")
                    _functions.Add(new ParseFunctionDeclaration(tokens));
                else
                {
                    _variables.Add(new ParseVariableDeclaration(tokens));
                    if (tokens.First() != ";")
                        ReportError("Expected ;");
                    tokens.RemoveAt(0);
                }
                    
            }
        }
        
        public static void ReportError(string error)
        {
            throw new Exception("Parser Error: " + error);
        }

        public override List<string> EmitInstructions()
        {
            List<string> result = new();

            foreach (var variable in _variables)
            {
                result.AddRange(variable.EmitInstructions());
            }

            foreach (var function in _functions)
            {
                result.AddRange(function.EmitInstructions());
            }

            return result;
        }
    }
}