using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace VirtualMachine.Peripherals
{
    public class DisplayPeripheral : IPeripheral
    {
        private uint _xPtr, _yPtr;
        private WriteableBitmap _bitmap;

        public DisplayPeripheral()
        {
            var mainWindow = new Window();
            var image = new Image();
            
            RenderOptions.SetBitmapScalingMode(image, BitmapScalingMode.NearestNeighbor);
            _bitmap = new WriteableBitmap(400, 300, 30, 30, PixelFormats.Rgb24, null);
            image.Source = _bitmap;
            
            mainWindow.Content = image;
            mainWindow.Show();
        }

        private void WritePixel(byte color, uint x, uint y)
        {
            var pixel = new[]{(byte) ((color >> 16) & 0xFF), (byte) ((color >>  8) & 0xFF), (byte) (color & 0xFF)};
            _bitmap.WritePixels(new Int32Rect((int) x, (int) y, 1, 1), pixel, 3, 0);
        }
        
        public byte? RunCycle()
        {
            return null;
        }

        public void AcknowledgeInterrupt(byte number)
        {
            return;
        }

        public void DoWrite(ushort value)
        {
            byte register = (byte) (value >> 12);

            switch (register)
            {
                case 0x0:
                    // Write pixel
                    var pixel = (byte) (value & 0xFF);
                    WritePixel(pixel, _xPtr, _yPtr);
                    
                    _xPtr += (uint) (value >> 8 & 0x03);
                    _yPtr += (uint) (value >> 10 & 0x03);
                    break;
                case 0x1:
                    // Write X pointer
                    _xPtr = (uint) value & 0x0FFF;
                    _xPtr %= 400;
                    break;
                case 0x2:
                    // Write Y pointer
                    _yPtr = (uint) value & 0x0FFF;
                    _yPtr %= 300;
                    break;
                case 0x3:
                    // Write control register
                    // TODO: Any control
                    break;
            }
        }

        public ushort DoRead()
        {
            return 0;
        }
    }
}