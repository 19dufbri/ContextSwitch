using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace SampleWpfApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public class MainWindow : Application
    {
        private readonly WriteableBitmap _bitmap;

        [STAThread]
        public static void Main()
        {
            var app = new MainWindow();
            app.Run();
        }
        
        public MainWindow()
        {
            var mainWindow = new Window();
            var image = new Image();
            
            RenderOptions.SetBitmapScalingMode(image, BitmapScalingMode.NearestNeighbor);
            _bitmap = new WriteableBitmap(32, 8, 30, 30, PixelFormats.Rgb24, null);
            image.Source = _bitmap;

            var pixels = new uint[8][];
            for (uint i = 0; i < 8; i++)
            {
                pixels[i] = new uint[32];
                for (uint j = 0; j < 8; j++)
                {
                    for (uint k = 0; k < 4; k++)
                    {
                        var red = 256 / 8 * i;
                        var green = (256 / 8 * j) << 8;
                        var blue = (256 / 4 * k) << 16;
                        pixels[i][j * 4 + k] = red | green | blue;
                    }
                }
            }

            DisplayBlock(pixels, 0, 0);

            mainWindow.Content = image;
            mainWindow.Show();
            MainWindow = mainWindow;
        }

        private void DisplayBlock(uint[][] pixels, int x, int y)
        {
            var width = pixels[0].Length;
            var height = pixels.Length;

            var result = new byte[width * height * 3];
            var i = 0;
            foreach (var row in pixels)
            {
                GenerateColors(row).CopyTo(result, i);
                i += width * 3;
            }
            
            _bitmap.WritePixels(new Int32Rect(x, y, width, height), result, 3*width, 0);
        }

        private static byte[] GenerateColors(uint[] pixels)
        {
            var result = new byte[pixels.Length * 3];

            for (var i = 0; i < pixels.Length; i++)
            {
                result[3*i+0] = (byte) ((pixels[i] >> 16) & 0xFF);
                result[3*i+1] = (byte) ((pixels[i] >>  8) & 0xFF);
                result[3*i+2] = (byte) (pixels[i] & 0xFF);
            }

            return result;
        }
    }
}