namespace VirtualMachine.Peripherals
{
    public interface IPeripheral
    {
        byte? RunCycle();
        void AcknowledgeInterrupt(byte number);
        void DoWrite(ushort value);
        ushort DoRead();
    }
}