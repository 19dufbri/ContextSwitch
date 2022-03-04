using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public class ParseFunctionCall : ParseStatement
    {
        private readonly string _functionName;
        private readonly List<ParseEqualityExpression> _functionArgs;

        public ParseFunctionCall(List<string> tokens)
        {
            _functionName = tokens.First();
            tokens.RemoveAt(0);
            if (tokens.First() != "(")
                ParseProgram.ReportError("Expected open parenthesis");
            tokens.RemoveAt(0);
            _functionArgs = GetFunctionArgs(tokens);
            if (tokens.First() != ")")
                ParseProgram.ReportError("Expected close parenthesis");
            tokens.RemoveAt(0);
        }
        
        private static List<ParseEqualityExpression> GetFunctionArgs(List<string> tokens)
        {
            var result = new List<ParseEqualityExpression>();
            
            var done = tokens.First() == ")";
            while (!done)
            {
                result.Add(new ParseEqualityExpression(tokens));
                if (tokens.First() != ",")
                    done = true;
                else
                    tokens.RemoveAt(0);
            }

            return result;
        }

        public override List<string> EmitInstructions()
        {
            List<string> result = new();
            
            result.Add("#CONTEXT");

            for (var i = 0; i < _functionArgs.Count; i++)
            {
                result.Add("#VARIABLE Func" + Guid + "n" + i);
            }

            // Calculate args and push to stack
            for (var i = 0; i < _functionArgs.Count; i++)
            {
                var argument = _functionArgs[i];
                result.AddRange(argument.EmitInstructions());
                result.Add("STI %R0, $Func" + Guid + "n" + i);
            }
            
            // Do the function
            result.Add("JSR $Routine" + _functionName);

            result.Add("#ENDCONTEXT");
            
            return result;
        }
    }
}