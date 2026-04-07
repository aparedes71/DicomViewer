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

        public BitmapSource DicomImage
        {
            get => _dicomImage;
            set
            {
                _dicomImage = value;
                OnPropertyChanged();
            }
        }

        public int WindowValue
        {
            get => _windowValue;
            set
            {
                _windowValue = value;
                OnPropertyChanged();
                ApplyWindowLevel();
            }
        }

        public int LevelValue
        {
            get => _levelValue;
            set
            {
                _levelValue = value;
                OnPropertyChanged();
                ApplyWindowLevel();
            }
        }

        public MainViewModel()
        {
            _dicomImageLoader = new DicomImageLoader();
            LoadImage(@"C:\Users\AndrewParedes\source\repos\DicomViewer\SampleData\MRBRAIN.DCM");
        }

        protected void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        protected void LoadImage(string imagePath = null)
        {
            if(imagePath != null)
            {
                _dicomImageData = _dicomImageLoader.LoadImage(imagePath);
                PixelFormat _pixelFormat = (_dicomImageData.BitFormat == 8) ? PixelFormats.Gray8 : PixelFormats.Gray16; //temporarily only supports these 2 can create a case later to support larger subset of formats
                int bytesPerPixel = (_pixelFormat == PixelFormats.Gray8) ? 1 : 2;

                if (_dicomImageData.BitFormat == 12)
                {
                    for (int i = 0; i < _dicomImageData.ImageData.Length; i++)
                    {
                        _dicomImageData.ImageData[i] = (ushort)(_dicomImageData.ImageData[i] << 4);
                    }
                }
                _originalImageData = (ushort[])_dicomImageData.ImageData.Clone();
                ApplyWindowLevel();
            }
        }

        private BitmapSource _dicomImage;
        private DicomImageLoader _dicomImageLoader;
        private DicomImageData _dicomImageData;
        private ushort[] _originalImageData;

        private int _windowValue = 34400;
        private int _levelValue = 17200;

        private void ApplyWindowLevel()
        {
            if (_originalImageData == null) return;

            ushort[] workingCopy = (ushort[])_originalImageData.Clone();

            ImageProcessingInterop.AdjustWindowLevel(workingCopy, _dicomImageData.Height, _dicomImageData.Width, _windowValue, _levelValue);
            PixelFormat pixelFormat = (_dicomImageData.BitFormat == 8) ? PixelFormats.Gray8 : PixelFormats.Gray16;
            int bytesPerPixel = (pixelFormat == PixelFormats.Gray8) ? 1 : 2;

            DicomImage = BitmapSource.Create(_dicomImageData.Width, _dicomImageData.Height, 96, 96, pixelFormat, null, workingCopy, _dicomImageData.Width * bytesPerPixel);
        }

    }
}
