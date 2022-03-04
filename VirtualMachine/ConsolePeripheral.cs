using System;

namespace VirtualMachine
{
    public class ConsolePeripheral : IPeripheral
    {
        public byte? RunCycle()
        {
            return null;
        }

        public void AcknowledgeInterrupt(byte number)
        {
            throw new NotImplementedException();
        }

        public void DoWrite(byte addr, ushort value)
        {
            Console.Write((char) value);
        }

        public ushort DoRead(byte addr)
        {
            throw new NotImplementedException();
        }
    }
}