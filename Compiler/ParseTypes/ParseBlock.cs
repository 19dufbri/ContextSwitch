using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public class ParseBlock : Parse
    {
        private readonly List<ParseStatement> _statements;
        public ParseBlock(List<string> tokens)
        {
            if (tokens.First() != "{")
                ParseProgram.ReportError("Expected `{`");
            tokens.RemoveAt(0);

            _statements = new List<ParseStatement>();
            while (tokens.Count > 0 && tokens.First() != "}")
            {
                _statements.Add(ParseStatement.Parse(tokens));
            }
            
            if (tokens.First() != "}")
                ParseProgram.ReportError("Expected `}`");
            tokens.RemoveAt(0);
        }

        public override List<string> EmitInstructions()
        {
            List<string> result = new();
            
            result.Add("#CONTEXT");
            foreach (var statement in _statements)
            {
                result.AddRange(statement.EmitInstructions());
            }
            result.Add("#ENDCONTEXT");

            return result;
        }
    }
}