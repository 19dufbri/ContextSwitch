using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public class ParseEqualityExpression : Parse
    {
        private readonly ParseRValue _lhs;
        private readonly bool _isEquality;
        private readonly ParseEqualityExpression _rhs;
        
        public ParseEqualityExpression(List<string> tokens)
        {
            _lhs = new ParseRValue(tokens);
            
            if (tokens.First() == "==" || tokens.First() == "!=")
            {
                _isEquality = tokens.First() == "==";
                tokens.RemoveAt(0);
                _rhs = new ParseEqualityExpression(tokens);
            }
        }
        
        public override List<string> EmitInstructions()
        {
            if (_rhs == null)
            {
                return _lhs.EmitInstructions();
            }

            var result = new List<string>();
            result.AddRange(_lhs.EmitInstructions());
            result.Add("#CONTEXT");
            result.Add("#VARIABLE Equal" + Guid);
            result.Add("STI %R0, $Equal" + Guid);
            result.AddRange(_rhs.EmitInstructions());
            result.Add("LFI $Equal" + Guid + ", %R2");
            result.Add("#ENDCONTEXT");

            if (!_isEquality)
            {
                result.Add("SUB %R2, %R0, %R0");
            }
            else
            {
                result.Add("SUB %R2, %R0, %R0");
                result.Add("LIR #0xffff, %R1");
                result.Add("XOR %R1, %R0, %R0");
            }
            return result;
        }
    }
}