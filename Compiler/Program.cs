using System;
using Compiler.ParseTypes;

namespace Compiler
{
    public static class Program
    {
        private static void Main(string[] args)
        {
            if (args.Length < 2) {
                Console.Error.WriteLine("Incorrect arguments");
                Console.Error.WriteLine("Usage: " + Environment.CommandLine + " <input file> <output file>");
                return;
            }

            // Initial tokenization stage
            var lines = System.IO.File.ReadAllLines(args[0]);
            var tokens = Tokenizer.TokenizeLines(lines);

            // Preprocessing
            var preprocessed = new Preprocessor(tokens, args[0]).Preprocess();

            // Parsing
            var parseTree = new ParseProgram(preprocessed);

            // Code generation
            var rawAssembly = parseTree.EmitInstructions();
            
            // Cook the assembly
            var cookedAssembly = PseudoAssembler.Process(rawAssembly);
            
            // Write to output file
            System.IO.File.WriteAllLines(args[1], cookedAssembly);
            
            // Goodbye
            Console.WriteLine("Compilation Successful");
        }
    }
}