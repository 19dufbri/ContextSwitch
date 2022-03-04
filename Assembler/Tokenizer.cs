using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace Assembler
{
    public class Tokenizer
    {
        private readonly List<string> _inlines = new();
        private string _currentString = "";
        private int _index;
        private int _lineNumber;
        public int PrintedLineNumber = 1;
        private ExpectedNext _nextType = ExpectedNext.Normal;

        public Tokenizer(IEnumerable<string> lines)
        {
            AddLines(lines);
        }

        public void AddLines(IEnumerable<string> lines)
        {
            _inlines.AddRange(lines);
        }

        private static readonly Regex WhitespaceRegex = new("\\s+");
        private static readonly Regex PreRegex = new("#[A-Z]+");
        private static readonly Regex PreArgRegex = new("\\s*([\\S]+|\".*\")");
        private static readonly Regex CommentRegex = new("--");
        private static readonly Regex LabelRegex = new("([A-Za-z0-9]+):");
        private static readonly Regex InstRegex = new("[A-Za-z]+");
        private static readonly Regex OpRegex = new("(\\$[A-Za-z]+|\\#0[xX][0-9a-fA-F]+|\\#-?[0-9]+|%[RS][0-7]),?");
        private static readonly Regex ByteRegex = new("\\.(0[xX][0-9a-fA-F]+|[0-9]+|'.')");
        private static readonly Regex StringRegex = new("\\.\"(.*)\"");

        // Return the current line and pointer for error reporting
        public string CurrentLine()
        {
            return "[" + PrintedLineNumber + ":" + _index + "]";
        }
        
        // Tokenize all lines of the input file
        public string NextToken()
        {
            // Process One Line
            while (_lineNumber < _inlines.Count)
            {

                var line = _inlines[_lineNumber];
                Match match;

                ChopWhitespace(line, ref _index);

                switch (_nextType)
                {
                    case ExpectedNext.Operand:
                        Match opMatch;
                        if ((opMatch = DoRegex(OpRegex, line, _index)) != null)
                        {
                            _index += opMatch.Length;
                            return opMatch.Groups[1].Value;
                        }
                        else
                        {
                            _index = 0;
                            _lineNumber++;
                            PrintedLineNumber++;
                            _nextType = ExpectedNext.Normal;
                            continue;
                        }
                    case ExpectedNext.PreArgument:
                        Match argMatch;
                        if ((argMatch = DoRegex(PreArgRegex, line, _index)) != null)
                        {
                            _index += argMatch.Length;
                            return argMatch.Groups[1].Value;
                        }
                        else
                        {
                            _index = 0;
                            _lineNumber++;
                            PrintedLineNumber++;
                            _nextType = ExpectedNext.Normal;
                            continue;
                        }
                    case ExpectedNext.StringPart:
                        if (_currentString.Length == 0)
                        {
                            _index = 0;
                            _lineNumber++;
                            PrintedLineNumber++;
                            _nextType = ExpectedNext.Normal;
                            return ".0";
                        }

                        var c = _currentString[0];
                        _currentString = _currentString[1..];
                        return ".'" + c + "'";
                }

                if (DoRegex(CommentRegex, line, _index) != null)
                {
                    // Just a comment, ignore rest of line
                    _lineNumber++;
                    PrintedLineNumber++;
                    _index = 0;
                    continue;
                }

                if ((match = DoRegex(PreRegex, line, _index)) != null)
                {
                    _index += match.Length;
                    _nextType = ExpectedNext.PreArgument;
                    return match.Value;
                }

                if ((match = DoRegex(LabelRegex, line, _index)) != null)
                {
                    // Label, only thing allowed on line
                    _index += match.Length;
                    return match.Value;
                }
                if ((match = DoRegex(InstRegex, line, _index)) != null)
                {
                    // Instruction, expect 0-3 operands
                    _index += match.Length;
                    _nextType = ExpectedNext.Operand;
                    return match.Value;
                }
                if ((match = DoRegex(ByteRegex, line, _index)) != null)
                {
                    _index += match.Length;
                    return match.Value;
                }
                if ((match = DoRegex(StringRegex, line, _index)) != null)
                {
                    var sMatch = match.Groups[1].Value;
                    _index += match.Length;
                    if (sMatch.Length == 0)
                        return ".0";
                    _currentString = sMatch[1..];
                    _nextType = ExpectedNext.StringPart;
                    return ".'" + sMatch[0] + '\'';
                }

                _index = 0;
                _lineNumber++;
                PrintedLineNumber++;
            }

            return null;
        }

        private static Match DoRegex(Regex regex, string line, int start)
        {
            if (!regex.IsMatch(line[start..])) return null;
            var match = regex.Match(line[start..]);
            return match.Index == 0 ? match : null;
        }

        private static void ChopWhitespace(string line, ref int i)
        {
            var match = DoRegex(WhitespaceRegex, line, i);
            i += match?.Length ?? 0;
        }

        private enum ExpectedNext
        {
            Normal,
            PreArgument,
            Operand,
            StringPart
        }

        public void ThrowError(string message)
        {
            throw Program.syntax_error(CurrentLine() + " " + message);
        }
    }
}