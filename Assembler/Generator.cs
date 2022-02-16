using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;

namespace Assembler
{
    public class Generator
    {
        private string _currentFile;
        private readonly Tokenizer _tokenizer;

        public Generator(Tokenizer tokenizer, string currentFile)
        {
            _tokenizer = tokenizer;
            _currentFile = currentFile;
        }

        public List<Label> LabelDef { get; } = new();
        public List<Label> LabelRef { get; } = new();
        public List<ushort> Output { get; } = new();

        public void GenerateAll()
        {
            while (!ProcessInstruction())
            {
            }
        }

        // Generate the instruction for each line of the input
        private bool ProcessInstruction()
        {
            ushort result;
            var token = _tokenizer.NextToken(); // Freed at end of proc_instr
            if (token == null) return true; // EOF, expected exit

            // Assembler Directives
            if (token.Equals("#CURFILE"))
            {
                _currentFile = _tokenizer.NextToken();
                _tokenizer.PrintedLineNumber = 1;
            }
            else if (token.Equals("#INCLUDE"))
            {
                // Include a file
                var fName = _tokenizer.NextToken(); // ~Should~ be a filename
                var newPath = Path.Combine(Path.GetDirectoryName(_currentFile) ?? "", fName);
                var tempLines = File.ReadAllLines(newPath);
                _tokenizer.AddLines(new[] {"#CURFILE " + newPath});
                _tokenizer.AddLines(tempLines);
            }

            // "Real" Instructions
            else if (token.Equals("ADD"))
            {
                // ADD r1, r2, r0
                var inA = expect_reg(); // r1
                var inB = expect_reg(); // r2
                var outReg = expect_reg(); // r0

                Output.Add(BaseInstructions.ADD(inA, inB, outReg));
            }
            else if (token.Equals("SUB"))
            {
                // SUB r1, r2, r0
                var inA = expect_reg(); // r1
                var inB = expect_reg(); // r2
                var outReg = expect_reg(); // r0
                
                Output.AddRange(BaseInstructions.SUB(inA, inB, outReg));
            }
            else if (token.Equals("LIL"))
            {
                // LIL i,  r0
                result = 0x2000;

                var t = next_num_or_label();
                if (t.Type == Parse.ParseType.Label)
                    LabelRef.Add(new Label(t.Name, (ushort) Output.Count, 0xFF, 0));
                else
                    result |= (ushort) (t.Number & 0xFF);

                result |= (ushort) (expect_reg() << 8);
                Output.Add(result);
            }
            else if (token.Equals("LIH"))
            {
                // LIH i,  r0
                result = 0x3000;

                var t = next_num_or_label();
                if (t.Type == Parse.ParseType.Label)
                    LabelRef.Add(new Label(t.Name, (ushort) Output.Count, 0xFF, 8));
                else
                    result |= (ushort) ((t.Number >> 8) & 0xFF);

                result |= (ushort) (expect_reg() << 8);
                Output.Add(result);
            }
            else if (token.Equals("STO"))
            {
                // STO r0, r1
                result = 0x4000;
                result |= (ushort) (expect_reg() << 8);
                result |= (ushort) (expect_reg() << 4);
                Output.Add(result);
            }
            else if (token.Equals("LOA"))
            {
                // LOA r1, r0
                result = 0x5000;
                result |= (ushort) (expect_reg() << 4);
                result |= (ushort) (expect_reg() << 8);
                Output.Add(result);
            }
            else if (token.Equals("MOV"))
            {
                // MOV r1, r0
                result = 0x6000;
                result |= (ushort) (expect_reg() << 4);
                result |= (ushort) (expect_reg() << 8);
                Output.Add(result);
            }
            else if (token.Equals("ADI"))
            {
                // ADI i,  r0
                result = 0x7000;

                var t = next_num_or_label();
                if (t.Type == Parse.ParseType.Label)
                    LabelRef.Add(new Label(t.Name, (ushort) Output.Count, 0xFF, 0));
                else
                    result |= (ushort) (t.Number & 0xFF);

                result |= (ushort) (expect_reg() << 8);
                Output.Add(result);
            }
            else if (token.Equals("SKL"))
            {
                // SKL r0, r1
                result = 0x8000;
                result |= (ushort) (expect_reg() << 8);
                result |= (ushort) (expect_reg() << 4);
                Output.Add(result);
            }
            else if (token.Equals("INT") || token.Equals("EIN"))
            {
                // INT/EIN
                result = 0x9000;
                Output.Add(result);
            }
            else if (token.Equals("IOR"))
            {
                // IOR i, r0
                result = 0xB000;

                var t = next_num_or_label();
                if (t.Type == Parse.ParseType.Label)
                    LabelRef.Add(new Label(t.Name, (ushort) Output.Count, 0xFF, 0));
                else
                    result |= (ushort) (t.Number & 0xFF);

                result |= (ushort) (expect_reg() << 8);
                Output.Add(result);
            }
            else if (token.Equals("IOW"))
            {
                // IOW r0, i
                result = 0xC000;
                result |= (ushort) (expect_reg() << 8);

                var t = next_num_or_label();
                if (t.Type == Parse.ParseType.Label)
                    LabelRef.Add(new Label(t.Name, (ushort) Output.Count, 0xFF, 0));
                else
                    result |= (ushort) (t.Number & 0xFF);

                Output.Add(result);
            }
            else if (token.Equals("AND"))
            {
                // ADD r1, r2, r0
                result = 0xD000;

                result |= (ushort) (expect_reg() << 4); // r1
                result |= expect_reg(); // r2
                result |= (ushort) (expect_reg() << 8); // r0

                Output.Add(result);
            }
            else if (token.Equals("XOR"))
            {
                // ADD r1, r2, r0
                result = 0xE000;

                result |= (ushort) (expect_reg() << 4); // r1
                result |= expect_reg(); // r2
                result |= (ushort) (expect_reg() << 8); // r0

                Output.Add(result);
            }
            else if (token.Equals("BRK"))
            {
                Output.Add(0xF000);
            }

            // Pseudo-Instructions
            else if (token == "RRO")
            {
                // Read register offset
                // RRO r1, r0, i

                var source = expect_reg();
                var destination = expect_reg();
                var offset = next_num_or_label();

                if (offset.Type != Parse.ParseType.Number)
                    Program.syntax_error("Expected number offset");

                // MOV r1, R5
                Output.Add((ushort) (0x6500 | (source << 4)));
                // ADI i, R5
                Output.Add((ushort) (0x7500 | (offset.Number & 0xFF)));
                // LOA r1+i, r0
                Output.Add((ushort) (0x5050 | (destination << 8)));
            }
            else if (token == "WRO")
            {
                // Write register offset
                // WRO r0, r1, i
                var source = expect_reg();
                var destination = expect_reg();
                var offset = next_num_or_label();

                if (offset.Type != Parse.ParseType.Number)
                    Program.syntax_error("Expected number offset");

                // MOV r1, R5
                Output.Add((ushort) (0x6500 | (destination << 4)));
                // ADI i, R5
                Output.Add((ushort) (0x7500 | (offset.Number & 0xFF)));
                // STO r0+i, r1
                Output.Add((ushort) (0x4050 | (source << 8)));
            }
            else if (token.Equals("LIR"))
            {
                // LIR ii, r0
                result = 0x2000;

                var t = next_num_or_label();

                result |= (ushort) (expect_reg() << 8);

                if (t.Type == Parse.ParseType.Label)
                {
                    LabelRef.Add(new Label(t.Name, (ushort) Output.Count, 0xFF, 0));
                    LabelRef.Add(new Label(t.Name, (ushort) (Output.Count + 1), 0xFF, 8));
                    Output.Add(result);
                    Output.Add((ushort) (result | 0x1000));
                }
                else
                {
                    Output.Add((ushort) (result | (t.Number & 0xFF)));
                    Output.Add((ushort) (result | 0x1000 | ((t.Number >> 8) & 0xFF)));
                }
            }
            else if (token.Equals("JMP"))
            {
                // JMP r1
                var register = ParseToRegister(next_num_or_label());

                result = 0x6700;
                result |= (ushort) (register << 4);
                Output.Add(result);
            }
            else if (token.Equals("JSR"))
            {
                // JSR r1
                var type = next_num_or_label();

                Output.Add(0x6570); // MOV PC, R5
                // ADI ?, R5 (4 if from register, 6 if label or number for hidden LIR)
                Output.Add((ushort) (type.Type == Parse.ParseType.Reg ? 0x7504 : 0x7506));
                Output.Add(0x4560); // STO R5, SP
                Output.Add(0x7601); // ADI 1,  SP

                var register = ParseToRegister(type);

                // MOV r1, PC
                result = (ushort) (0x6700 | (register << 4));
                Output.Add(result);
            }
            else if (token.Equals("PSH"))
            {
                // PSH r0 
                result = 0x4060;

                result |= (ushort) (expect_reg() << 8);

                Output.Add(result); // STO r0, SP
                Output.Add(0x7601); // ADI 1,  SP
            }
            else if (token.Equals("POP"))
            {
                // POP r0 
                Output.Add(0x76FF); // ADI -1, SP

                result = 0x5060;

                result |= (ushort) (expect_reg() << 8);

                Output.Add(result); // LOA SP, r0
            }
            else if (token.Equals("JGT"))
            {
                // JGT r0, r1, r2
                // If r0 > r1, goto r2

                // SKL r0, r1
                result = 0x8000;
                result |= (ushort) (expect_reg() << 8);
                result |= (ushort) (expect_reg() << 4);
                
                var register = ParseToRegister(next_num_or_label());
                
                Output.Add(result);

                // MOV r2, PC
                result = 0x6700;
                result |= (ushort) (register << 4);
                Output.Add(result);
            }
            else if (token.Equals("RET"))
            {
                // RET
                // POP return address and jump
                Output.Add(0x76FF); // ADI		-1, 	SP
                Output.Add(0x5760); // LOA 	SP,		PC
            }

            else if (token[0] == '.')
            {
                // Direct Byte
                if (token[1] >= '0' && token[1] <= '9')
                {
                    result = ParseNumber(token[1..]);
                }
                else if (token[1] == '\'' && token[3] == '\'')
                {
                    // A character
                    result = (ushort) (token[2] & 0xFF);
                }
                else
                {
                    throw Program.syntax_error("Expected number or single quoted character");
                }

                Output.Add(result);
            }
            else if (token[^1] == ':')
            {
                // Label Definitions
                LabelDef.Add(new Label(token[..^1], (ushort) Output.Count, 0, 0));
            }
            else
            {
                Console.Error.WriteLine(" >> " + token + " <<");
                throw Program.syntax_error("Unknown token");
            }

            return false;
        }

        private byte expect_reg()
        {
            var r0 = next_num_or_label();
            if (r0.Type != Parse.ParseType.Reg)
            {
                Console.Error.WriteLine(r0.Type);
                Program.syntax_error("Register Expected");
            }

            var res = (byte) r0.Number;
            return res; // r0
        }

        // Get the next number or label
        private Parse next_num_or_label()
        {
            var token = _tokenizer.NextToken(); // Freed at end of func

            var result = token[0] switch
            {
                '#' => new Parse(Parse.ParseType.Number, ParseNumber(token[1..])),
                '$' => new Parse(token[1..]),
                '%' => new Parse(Parse.ParseType.Reg, GetRegister(token)),
                _ => throw Program.syntax_error("Unknown reference!")
            };

            return result;
        }

        private static ushort ParseNumber(string token)
        {
            // Handle hex
            if (token.Length >= 3 && (token[..2].Equals("0x") || token[..2].Equals("0X")))
                return Convert.ToUInt16(token[2..], 16);
            return (ushort) short.Parse(token, NumberStyles.Any);
        }

        // Get Single Register
        private static byte GetRegister(string token)
        {
            byte result = 0x00;

            if (token.Length != 3 || token[0] != '%')
            {
                Console.Error.WriteLine(" >> Expected register, got " + token + " <<");
                Program.syntax_error("Unknown Register");
            }

            switch (token[1])
            {
                case 'R':
                    break;
                case 'S':
                    result |= 0x08;
                    break;
                default:
                    Console.Error.WriteLine(" >> " + token + " <<");
                    Program.syntax_error("Unknown Register");
                    break;
            }

            if (token[2] >= '0' && token[2] <= '9')
            {
                result |= (byte) (token[2] - '0');
            }
            else
            {
                Console.Error.WriteLine(" >> " + token + " <<\n");
                Program.syntax_error("Unknown Register");
            }

            return result;
        }

        private ushort ParseToRegister(Parse value)
        {
            ushort result;
            ushort register = 0x5;
            switch (value.Type)
            {
                case Parse.ParseType.Reg:
                    register = value.Number;
                    break;
                case Parse.ParseType.Label:
                    result = 0x2500;
                    LabelRef.Add(new Label(value.Name, (ushort) Output.Count, 0xFF, 0));
                    LabelRef.Add(new Label(value.Name, (ushort) (Output.Count + 1), 0xFF, 8));
                    Output.Add(result);
                    Output.Add((ushort) (result | 0x1000));
                    break;
                case Parse.ParseType.Number:
                    result = 0x2500;
                    Output.Add((ushort) (result | (value.Number & 0xFF)));
                    Output.Add((ushort) (result | 0x1000 | ((value.Number >> 8) & 0xFF)));
                    break;
                default:
                    Program.syntax_error("Expected Register, Label, or Number!");
                    break;
            }

            return register;
        }
    }
}