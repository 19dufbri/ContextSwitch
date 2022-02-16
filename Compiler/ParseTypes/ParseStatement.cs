using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public abstract class ParseStatement : Parse
    {
        public static ParseStatement Parse(List<string> tokens)
        {
            ParseStatement result = null;
            var token = tokens.First();
            if (token == "if")
            {
                result = new ParseIf(tokens);
            }
            else if (token == "return")
            {
                result = new ParseReturn(tokens);
                if (tokens.First() != ";")
                    ParseProgram.ReportError("Expected semicolon");
                tokens.RemoveAt(0);
            }
            else if (token == "while")
            {
                result = new ParseWhile(tokens);
            }
            else if (token.All(char.IsLetter))
            {
                result = ParseVarOrCall(tokens);
                if (tokens.First() != ";")
                    ParseProgram.ReportError("Expected semicolon");
                tokens.RemoveAt(0);
            }
            else
            {
                ParseProgram.ReportError("Expected something, I'm tired");
            }
            
            return result;
        }

        private static ParseStatement ParseVarOrCall(List<string> tokens)
        {
            // Identifier or type
            if (tokens[1].All(char.IsLetter))
            {
                // Must be var declaration
                return new ParseVariableDeclaration(tokens);
            }
            switch (tokens[1])
            {
                case "=":
                    return new ParseVariableAssignment(tokens);
                case "(":
                    return new ParseFunctionCall(tokens);
                default:
                    ParseProgram.ReportError("Expected something");
                    return null;
            }
        }
    }
}