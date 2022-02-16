namespace VirtualMachine
{
    public interface IPeripheral
    {
        byte? RunCycle();
        void AcknowledgeInterrupt(byte number);
        void DoWrite(byte addr, ushort value);
        ushort DoRead(byte addr);
    }
}