using System;
using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public class ParseIf : ParseStatement
    {
        private readonly ParseRValue _condition;
        private readonly Parse _blockOrStatement;
        private readonly Parse _elseBlockOrStatement;
        
        public ParseIf(List<string> tokens)
        {
            if (tokens.First() != "if")
                ParseProgram.ReportError("Expected if statement");
            tokens.RemoveAt(0);
            
            if (tokens.First() != "(")
                ParseProgram.ReportError("Expected open parens");
            tokens.RemoveAt(0);

            _condition = new ParseRValue(tokens);
            
            if (tokens.First() != ")")
                ParseProgram.ReportError("Expected close parens");
            tokens.RemoveAt(0);
            
            if (tokens.First() == "{")
                _blockOrStatement = new ParseBlock(tokens);
            else
                _blockOrStatement = ParseTypes.ParseStatement.Parse(tokens);
            
            if (tokens.First() == "else")
            {
                tokens.RemoveAt(0);
                if (tokens.First() == "{")
                    _elseBlockOrStatement = new ParseBlock(tokens);
                else
                    _elseBlockOrStatement = ParseTypes.ParseStatement.Parse(tokens);
            }
        }

        public override List<string> EmitInstructions()
        {
            List<string> result = new();
            
            result.AddRange(_condition.EmitInstructions());
            
            // Jump to else if we have one, otherwise to end
            result.Add("LIR $IfBody" + Guid + ", %R2");
            
            // If result != 0
            result.Add("LIR #1, %R1");
            result.Add("JGT %R0, %R1, %R2");
            result.Add("LIR #-1, %R1");
            result.Add("JGT %R1, %R0, %R2");
            
            // Emit instructions for else block
            if (_elseBlockOrStatement != null)
                result.AddRange(_elseBlockOrStatement.EmitInstructions());

            // Jump on else condition
            result.Add("LIR $EndIf" + Guid + ", %R0");
            result.Add("JMP %R0");

            // Body of the if statement
            result.Add("IfBody" + Guid + ":");
            result.AddRange(_blockOrStatement.EmitInstructions());

            result.Add("EndIf" + Guid + ":");
            return result;
        }
    }
}