using System;
using System.Collections.Generic;
using System.Linq;

namespace Compiler.ParseTypes
{
    public class ParseRValue : Parse
    {
        private enum RValueType
        {
            Addition,
            Subtraction,
            And,
            Value
        }

        private readonly RValueType _valueType;
        private readonly ParseValue _left;
        private readonly ParseRValue _right;

        public ParseRValue(List<string> tokens)
        {
            // Valid values are:
            //  value + rvalue
            //  value - rvalue
            //  value
            _left = new ParseValue(tokens);
            var token = tokens.First();
            switch (token)
            {
                case "&":
                    tokens.RemoveAt(0);
                    _right = new ParseRValue(tokens);
                    _valueType = RValueType.And;
                    break;
                case "+":
                    // Addition
                    tokens.RemoveAt(0);
                    _right = new ParseRValue(tokens);
                    _valueType = RValueType.Addition;
                    break;
                case "-":
                    // Subtraction
                    tokens.RemoveAt(0);
                    _right = new ParseRValue(tokens);
                    _valueType = RValueType.Subtraction;
                    break;
                default:
                    // Bare value
                    _valueType = RValueType.Value;
                    break;
            }
        }

        public override List<string> EmitInstructions()
        {
            var result = _left.EmitInstructions();

            switch (_valueType)
            {
                case RValueType.And:
                    result.Add("#CONTEXT");
                    result.Add("#VARIABLE And" + Guid);
                    result.Add("STI %R0, $And" + Guid);
                    result.AddRange(_right.EmitInstructions());
                    result.Add("LFI $And" + Guid + ", %R2");
                    result.Add("#ENDCONTEXT");
                    result.Add("AND %R2, %R0, %R0");
                    break;
                case RValueType.Addition:
                    result.Add("#CONTEXT");
                    result.Add("#VARIABLE Add" + Guid);
                    result.Add("STI %R0, $Add" + Guid);
                    result.AddRange(_right.EmitInstructions());
                    result.Add("LFI $Add" + Guid + ", %R2");
                    result.Add("#ENDCONTEXT");
                    result.Add("ADD %R2, %R0, %R0");
                    break;
                case RValueType.Subtraction:
                    result.Add("#CONTEXT");
                    result.Add("#VARIABLE Add" + Guid);
                    result.Add("STI %R0, $Add" + Guid);
                    result.AddRange(_right.EmitInstructions());
                    result.Add("LFI $Add" + Guid + ", %R2");
                    result.Add("#ENDCONTEXT");
                    result.Add("SUB %R2, %R0, %R0");
                    break;
                case RValueType.Value:
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
            
            return result;
        }
    }
}