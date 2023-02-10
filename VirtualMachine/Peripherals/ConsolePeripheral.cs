using System;

namespace VirtualMachine.Peripherals
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

        public void DoWrite(ushort value)
        {
            Console.Write((char) value);
        }

        public ushort DoRead()
        {
            throw new NotImplementedException();
        }
    }
}