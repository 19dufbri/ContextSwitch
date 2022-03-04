using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public class ParseWhile : ParseStatement
    {
        private readonly ParseEqualityExpression _condition;
        private readonly Parse _blockOrStatement;
        
        public ParseWhile(List<string> tokens)
        {
            if (tokens.First() != "while")
                ParseProgram.ReportError("Expected while statement");
            tokens.RemoveAt(0);
            
            if (tokens.First() != "(")
                ParseProgram.ReportError("Expected open parens");
            tokens.RemoveAt(0);

            _condition = new ParseEqualityExpression(tokens);
            
            if (tokens.First() != ")")
                ParseProgram.ReportError("Expected close parens");
            tokens.RemoveAt(0);
            
            if (tokens.First() == "{")
                _blockOrStatement = new ParseBlock(tokens);
            else
                _blockOrStatement = ParseTypes.ParseStatement.Parse(tokens);
        }

        public override List<string> EmitInstructions()
        {
            List<string> result = new();
            
            // Condition check, result on %R0
            result.Add("While" + Guid + ":");
            result.AddRange(_condition.EmitInstructions());

            // While result != 0
            result.Add("LIR $WhileBody" + Guid + ", %R2");
            result.Add("LIR #1, %R1");
            result.Add("JGT %R0, %R1, %R2");
            result.Add("LIR #-1, %R1");
            result.Add("JGT %R1, %R0, %R2");
            result.Add("JMP $EndWhile" + Guid);
            
            // While body
            result.Add("WhileBody" + Guid + ":");
            result.AddRange(_blockOrStatement.EmitInstructions());
            result.Add("JMP $While" + Guid);

            // End of the loop
            result.Add("EndWhile" + Guid + ":");
            return result;
        }
    }
}