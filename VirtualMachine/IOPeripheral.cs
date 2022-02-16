namespace VirtualMachine
{
    public class IOPeripheral : IPeripheral
    {
        public byte? RunCycle()
        {
            throw new System.NotImplementedException();
        }

        public void AcknowledgeInterrupt(byte number)
        {
            throw new System.NotImplementedException();
        }

        public void DoWrite(byte addr, ushort value)
        {
            throw new System.NotImplementedException();
        }

        public ushort DoRead(byte addr)
        {
            throw new System.NotImplementedException();
        }
    }
}