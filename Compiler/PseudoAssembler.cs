using System;
using System.Collections.Generic;
using System.Linq;

namespace Compiler
{
    public static class PseudoAssembler
    {
        private static readonly string[] Bootstrap = {
            "LIR #0xF000, %R6",
            "JSR $Routinemain",
            "IOW %R0, #0xFF"
        };

        public static List<string> Process(List<string> input)
        {
            List<string> result = new();    // Resulting stream
            List<string> strings = new();   // Strings to include
            List<string> globals = new();   // Global variable names

            // Process strings
            for (var i = 0; i < input.Count; i++)
            {
                var line = input[i];
                if (line.StartsWith("#STRING"))
                {
                    input.RemoveAt(i);
                    var str = line[9..^1];
                    strings.Add(str);
                }
            }

            // Process functions and globals
            while (input.Count > 0)
            {
                var line = input.First();
                if (line.StartsWith("#FUNCTION"))
                {
                    result.AddRange(ProcessFunction(input));
                }
                else if (line.StartsWith("#VARIABLE"))
                {
                    input.RemoveAt(0);
                    globals.Add(line[10..]);
                }
                else
                {
                    throw new Exception("Expected #VARIABLE or #FUNCTION on top level");
                }
            }

            // Output locations of global variables
            foreach (var global in globals)
            {
                result.Add("Variable" + global + ":");
                result.Add(".0");
            }

            // Process the global variables
            result = TryGlobalVariables(result);
            
            // Output strings at end of file
            foreach (var s in strings)
            {
                var hash = Math.Abs(s.GetHashCode()).ToString().ToCharArray();
                var hashString = hash.Aggregate("", (current, t) => current + (char) (t - '0' + 'A'));
                result.Add("String" + hashString + ":");
                result.Add(".\"" + s + "\"");
            }
            
            result.InsertRange(0, Bootstrap);

            return result;
        }

        private static List<string> ProcessFunction(List<string> input)
        {
            List<string> args = new();

            // Starts with #FUNCTION
            var token = input.First();
            if (!token.StartsWith("#FUNCTION"))
                throw new Exception("Expected #FUNCTION");
            input.RemoveAt(0);

            // Record all params
            while (input.First().StartsWith("#VARIABLE"))
            {
                token = input.First();
                input.RemoveAt(0);
                args.Add(token[10..]);
            }
            
            // Ends with #ENDPARAMS
            if (input.First() != "#ENDPARAMS")
                throw new Exception("Expected #ENDPARAMS");
            input.RemoveAt(0);

            List<string> result = new();
            
            // If we have a definition, include it
            if (input.Count > 0 && input.First() == "#FUNCTIONDEF")
            {
                input.RemoveAt(0);
                result.Add(input.First());
                input.RemoveAt(0); // Label for Function
                var varsPadded = new List<string>();
                varsPadded.AddRange(args);
                varsPadded.Add("PaddingPleaseIgnore");
                result.AddRange(ProcessContext(input, varsPadded, args.Count));
                
                // TODO: Janky solution, fix
                var popCount = result.Count - result.LastIndexOf("RET") - 1;
                for (var i = 0; i < popCount; i++)
                {
                    result.RemoveAt(result.Count - 1);
                    result.Insert(result.LastIndexOf("RET"), "POP %R5");
                }
            }
            return result;
        }

        private static List<string> ProcessContext(List<string> input, List<string> oldVars, int nArgs)
        {
            List<string> result = new();
            
            // Original #CONTEXT
            input.RemoveAt(0);
            
            // Variables in this context
            var vars = new List<string>();
            
            // Count of nests
            var nestedCount = 1;

            // Find all the variables on our context level
            var i = 0;
            while (nestedCount > 0)
            {
                var line = input[i++];
                switch (line)
                {
                    case "#CONTEXT":
                        nestedCount++;
                        break;
                    case "#ENDCONTEXT":
                        nestedCount--;
                        break;
                }

                // Variable on our context level, remove it and record
                if (nestedCount == 1 && line.StartsWith("#VARIABLE"))
                {
                    i--;
                    input.RemoveAt(i);
                    vars.Add(line[10..]);
                }
            }

            // Allocate space for block variables
            for (var j = 0; j < vars.Count; j++)
            {
                result.Add("PSH %R0");
            }
            
            var newVars = new List<string>();
            
            newVars.AddRange(oldVars);
            newVars.AddRange(vars);

            // Recurse
            while (input.Count > 0)
            {
                var line = input.First();
                if (line == "#CONTEXT")
                {
                    result.AddRange(ProcessContext(input, newVars, nArgs));
                }
                else if (line == "#ENDCONTEXT")
                {
                    input.RemoveAt(0);
                    break;
                }
                else
                {
                    // Remove token and add to result
                    input.RemoveAt(0);
                    result.Add(line);
                }
            }
            
            for (var j = 0; j < vars.Count; j++)
            {
                result.Add("POP %R5");
            }

            // Try to resolve variables in this block
            result = TryOffsetVariables(result, newVars, nArgs);
            
            return result;
        }

        private static List<string> TryOffsetVariables(List<string> lines, List<string> vars, int argCount)
        {
            List<string> result = new();

            var offset = -vars.Count;

            foreach (var line in lines)
            {

                if (line.StartsWith("LFI"))
                {
                    var identEnd = line[4..].IndexOf(',');
                    var ident = line[5..][..identEnd][..^1];
                    var reg = line[6..][identEnd..];
                    var identIndex = vars.FindIndex(v => v == ident);
                    if (identIndex >= 0)
                        result.Add("RRO %R6, " + reg + ", #" + (offset + identIndex));
                    else
                        result.Add(line);
                }
                else if (line.StartsWith("STI"))
                {
                    var identEnd = line[4..].IndexOf(',');
                    var reg = line[4..][..identEnd];
                    var ident = line[7..][identEnd..];
                    var identIndex = vars.FindIndex(v => v == ident);
                    if (identIndex >= 0)
                        result.Add("WRO " + reg + ", %R6, #" + (offset + identIndex));
                    else
                        result.Add(line);
                }
                else
                {
                    result.Add(line);
                }
            }

            return result;
        }
        
        private static List<string> TryGlobalVariables(List<string> lines)
        {
            List<string> result = new();

            foreach (var line in lines)
            {

                if (line.StartsWith("LFI"))
                {
                    var identEnd = line[4..].IndexOf(',');
                    var ident = line[4..][..identEnd];
                    var reg = line[7..][identEnd..];
                    result.Add("LIR $Variable" + ident + ", %R5");
                    result.Add("LOA %R5, " + reg);
                }
                else if (line.StartsWith("STI"))
                {
                    var identEnd = line[4..].IndexOf(',');
                    var reg = line[4..][..identEnd];
                    var ident = line[7..][identEnd..];
                    result.Add("LIR $Variable" + ident + ", %R5");
                    result.Add("STO " + reg + ", %R5");
                }
                else
                {
                    result.Add(line);
                }
            }

            return result;
        }
    }
}