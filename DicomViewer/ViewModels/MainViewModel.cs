using DicomViewer.Models;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;

namespace DicomViewer.ViewModels
{
    public class MainViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        protected void LoadImage(string imagePath = null)
        {
            if(imagePath != null)
            {
                _dicomImageData = _dicomImageLoader.LoadImage(imagePath);
                PixelFormat _pixelFormat = (_dicomImageData.BitFormat == 8) ? PixelFormats.Gray8 : PixelFormats.Gray16; //temporaily only supports these 2 can create a case later to support larger subset of formats
                int bytesPerPixel = (_pixelFormat == PixelFormats.Gray8) ? 1 : 2;
                //Need to scale 12 bit values to 16 bit otherwise image just appears black TODO: fix hardcoded 12 since will not always be 12 and scaling will need to be calculated intelligently
                if (_dicomImageData.BitFormat == 12)
                {
                    byte[] scaledData = new byte[_dicomImageData.ImageData.Length];
                    for (int i = 0; i < scaledData.Length; i += 2)
                    {
                        ushort pixel = BitConverter.ToUInt16(_dicomImageData.ImageData, i);
                        pixel = (ushort)(pixel << 4);
                        Buffer.BlockCopy(BitConverter.GetBytes(pixel), 0, scaledData, i, 2);
                    }
                    _dicomImage = BitmapSource.Create(_dicomImageData.Width, _dicomImageData.Height, 96, 96, _pixelFormat, null, scaledData, _dicomImageData.Width * bytesPerPixel);
                }
                else
                {
                    _dicomImage = BitmapSource.Create(_dicomImageData.Width, _dicomImageData.Height,96,96,_pixelFormat,null,_dicomImageData.ImageData,_dicomImageData.Width * bytesPerPixel);
                }
            }
        }

        private BitmapSource _dicomImage;
        private DicomImageLoader _dicomImageLoader;
        private DicomImageData _dicomImageData;
        public BitmapSource DicomImage
        {
            get => _dicomImage;
            set
            {
                _dicomImage = value;
                OnPropertyChanged();
            }
        }

        public MainViewModel()
        {
            _dicomImageLoader = new DicomImageLoader();
            LoadImage(@"C:\Users\AndrewParedes\source\repos\DicomViewer\SampleData\MRBRAIN.DCM");
        }
    }
}
