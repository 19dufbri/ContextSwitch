using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace Compiler
{
    public static class Tokenizer
    {
	    private static void ChopWhitespace(string line, ref int i)
	    {
		    while(line.Length > i && char.IsWhiteSpace(line, i)) i++;
	    }

	    // Tokenize a whole file
		public static List<string> TokenizeLines(IEnumerable<string> lines) {
			var result = new List<string>();

			foreach (var line in lines)
			{
				int used;
				var i = 0;

				ChopWhitespace(line, ref i);
				while ((used = NextToken(line[i..], result)) != 0) {
					i += used;
					ChopWhitespace(line, ref i);
				}
			}
			return result;
		}

		private static Match DoRegex(Regex regex, string line, ref int start)
		{
			if (!regex.IsMatch(line[start..])) return null;
			var match = regex.Match(line[start..]);
			if (match.Index != 0) return null;
			start += match.Length;
			return match;
		}

		private static int NextToken(string line, ICollection<string> tokens) {
			var i = 0;
			
			ChopWhitespace(line, ref i);
			
			var identRegex = new Regex("[A-Za-z]+");
			var symbolRegex = new Regex("[<>=]=|\\|\\|?|&&?|[;{}(),=!<+\\-*&]");
			var stringRegex = new Regex("\".*\"");
			var numberRegex = new Regex("0[xX][0-9A-Fa-f]+|\\d+|'\\?[^\\\\]'");	// Decimal, hex, and chars
			var includeRegex = new Regex("#([A-Za-z]+)");
			var preArgRegex = new Regex("([\\S]+|\".*\")");

			Match match;
			if ((match = DoRegex(includeRegex, line, ref i)) != null)
			{
				tokens.Add("#");
				tokens.Add(match.Groups[1].Value);
				while (i < line.Length)
				{
					ChopWhitespace(line, ref i);
					Match argMatch;
					if ((argMatch = DoRegex(preArgRegex, line, ref i)) != null)
					{
						var arg = argMatch.Groups[1].Value[0] == '"' ? argMatch.Groups[1].Value[1..^1] : argMatch.Groups[1].Value;
						tokens.Add(arg);
					}
					else
					{
						break;
					}
				}
			}
			else if ((match = DoRegex(numberRegex, line, ref i)) != null)
			{
				tokens.Add(match.Value);
			}
			else if ((match = DoRegex(identRegex, line, ref i)) != null)
			{
				tokens.Add(match.Value);
			}
			else if ((match = DoRegex(symbolRegex, line, ref i)) != null)
			{
				tokens.Add(match.Value);
			}
			else if ((match = DoRegex(stringRegex, line, ref i)) != null)
			{
				tokens.Add(match.Value);
			}
			else if (line != "")
			{
				TokenizeError("We didn't match anything? Got: \"" + line[i..] + "\"");
			}

			return i;
		} 

		// Should be a very rare case
		private static void TokenizeError(string message) {
			Console.Error.WriteLine(message);
			Environment.Exit(-1);
		}
    }
}