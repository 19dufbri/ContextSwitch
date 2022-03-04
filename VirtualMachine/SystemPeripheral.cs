namespace VirtualMachine
{
    public class SystemPeripheral : IPeripheral
    {
        public byte? RunCycle()
        {
            return null;
        }

        public void AcknowledgeInterrupt(byte number)
        {
            throw new System.NotImplementedException();
        }

        public void DoWrite(byte addr, ushort value)
        {
            System.Environment.Exit(value);
        }

        public ushort DoRead(byte addr)
        {
            throw new System.NotImplementedException();
        }
    }
}