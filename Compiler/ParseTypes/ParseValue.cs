using System;
using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public class ParseValue : Parse
    {
        private enum ParseValueType
        {
            Constant,
            Parenthesis,
            Identifier,
            FunctionCall
        }

        private readonly ParseValueType _valueType;
        private readonly string _constOrIdentValue;
        private readonly ParseEqualityExpression _parenContent;
        private readonly ParseFunctionCall _functionCall;
        
        public ParseValue(List<string> tokens)
        {
            // Rvalues can be:
            //  ( <rvalue> )
            //  Constant
            //  ID
            var token = tokens.First();
            if (char.IsDigit(token[0]) || token[0] == '"')
            {
                // Constant
                tokens.RemoveAt(0);
                _valueType = ParseValueType.Constant;
                _constOrIdentValue = token;
            }
            else if (token == "(")
            {
                // Parenthesis
                tokens.RemoveAt(0);
                _valueType = ParseValueType.Parenthesis;
                _parenContent = new ParseEqualityExpression(tokens);
                token = tokens.First();
                tokens.RemoveAt(0);
                if (token != ")")
                    ParseProgram.ReportError("Expected closed parenthesis");
            }
            else if (tokens[1] == "(")
            {
                // Function call
                _valueType = ParseValueType.FunctionCall;
                _functionCall = new ParseFunctionCall(tokens);
            }
            else if (token.All(char.IsLetter))
            {
                // Identifier
                tokens.RemoveAt(0);
                _valueType = ParseValueType.Identifier;
                _constOrIdentValue = token;
            }
            else
            {
                ParseProgram.ReportError("Expected value `value`");
            }
        }

        public override List<string> EmitInstructions()
        {
            List<string> result = new();

            switch (_valueType)
            {
                case ParseValueType.Constant:
                    if (_constOrIdentValue[0] == '"')
                    {
                        var hash = Math.Abs(_constOrIdentValue[1..^1].GetHashCode()).ToString().ToCharArray();
                        var hashString = hash.Aggregate("", (current, t) => current + (char) (t - '0' + 'A'));
                        result.Add("#STRING " + _constOrIdentValue);
                        result.Add("LIR $String" + hashString + ", %R0");
                    }
                    else
                    {
                        result.Add("LIR #" + _constOrIdentValue + ", %R0");
                    }
                    break;
                case ParseValueType.Identifier:
                    result.Add("LFI $" + _constOrIdentValue + ", %R0");
                    break;
                case ParseValueType.Parenthesis:
                    result.AddRange(_parenContent.EmitInstructions());
                    break;
                case ParseValueType.FunctionCall:
                    result.AddRange(_functionCall.EmitInstructions());
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            return result;
        }
    }
}