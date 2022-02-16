namespace Assembler
{
    public static class BaseInstructions
    {
        public static ushort ADD(byte inA, byte inB, byte outReg)
        {
            return (ushort) (0x0000 | (inA << 4) | inB | (outReg << 8));
        }
        
        public static ushort XOR(byte inA, byte inB, byte outReg)
        {
            return (ushort) (0xE000 | (inA << 4) | inB | (outReg << 8));
        }

        public static ushort[] SUB(byte inA, byte inB, byte outReg)
        {
            var result = new ushort[7];
            
            LIR(5, 0xFFFF).CopyTo(result, 0);
            result[2] = XOR(inB, 5, 5);
            LIR(outReg, 0x0001).CopyTo(result, 3);
            result[5] = ADD(5, outReg, 5);
            result[6] = ADD(inA, 5, outReg);

            return result;
        }
        
        public static ushort LIL(byte reg, byte value)
        {
            return (ushort) (0x2000 | (reg << 8) | value);
        }
        
        public static ushort LIH(byte reg, byte value)
        {
            return (ushort) (0x3000 | (reg << 8) | value);
        }

        public static ushort[] LIR(byte reg, ushort value)
        {
            return new[] {LIL(reg, (byte) (value & 0xFF)), LIH(reg, (byte) ((value >> 8) & 0xFF))};
        }
    }
}