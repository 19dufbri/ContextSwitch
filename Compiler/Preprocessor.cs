using System;
using System.Collections.Generic;
using System.IO;

namespace Compiler
{
    public class Preprocessor
    {

        private readonly List<string> _result;
        private readonly List<string> _tokens;
        private string _currentFile;
        private int _index;

        public Preprocessor(List<string> inTokens, string filename)
        {
            _result = new List<string>();
            _tokens = inTokens;
            _currentFile = filename;
        }
        
        // Preprocess all the tokens in a list and clear the list
        public List<string> Preprocess()
        {
            // Normal preprocessor tings
            while (_index < _tokens.Count)
            {
                var token = _tokens[_index++];
                if (token.Equals("#"))
                {
                    ProcessPre();
                }
                else
                {
                    _result.Add(token);
                }
            }

            return _result;
        }
        
        private string NextToken()
        {
            return _tokens.Count >= _index ? _tokens[_index++] : null;
        }

        // Get token and remove it from the iterator, normal operation
        private string ExpectTokenPre() {
            var token = NextToken();
            if (token == null) {
                PreprocessError("Unexpected EOF!");
            }
            return token;
        }

        // Found a preprocessor directive
        private void ProcessPre()
        {
            var token = ExpectTokenPre();

            switch (token)
            {
                case "currentfile":
                    _currentFile = NextToken();
                    break;
                case "include":
                {
                    // Header file
                    // Load contents of new file at end
                    // TODO: include at most once (no circular dependancies)
                    // Assembler Directives
                    // Include a file
                    var fName = NextToken(); // ~Should~ be a filename
                    var newPath = Path.Combine(Path.GetDirectoryName(_currentFile) ?? "", fName);
                    var tempLines = File.ReadAllLines(newPath);
                    _tokens.Add("#");
                    _tokens.Add("currentfile");
                    _tokens.Add(newPath);
                    _tokens.AddRange(Tokenizer.TokenizeLines(tempLines));
                    break;
                }
            }
        }

        private static void PreprocessError(string message) {
            Console.Error.WriteLine("Error in preprocessor:\n");
            Console.Error.WriteLine("\t>> " + message + " <<\n");
            Environment.Exit(-1);
        }
    }
}