namespace VirtualMachine.Peripherals
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

        public void DoWrite(ushort value)
        {
            System.Environment.Exit(value);
        }

        public ushort DoRead()
        {
            throw new System.NotImplementedException();
        }
    }
}