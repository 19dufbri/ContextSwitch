namespace Assembler
{
    public class Label
    {
        public Label(string name, ushort addr, ushort mask, byte shift)
        {
            Name = name;
            Addr = addr;
            Mask = mask;
            Shift = shift;
        }

        public string Name { get; }
        public ushort Addr { get; }
        public ushort Mask { get; }
        public byte Shift { get; }
    }
}