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
    /// <summary>
    /// ViewModel for the main window. Owns image loading, window/level state,
    /// and the conversion from raw pixel data to a WPF-displayable BitmapSource.
    /// </summary>
    public class MainViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>The processed image displayed in the UI, updated on every window/level change.</summary>
        public BitmapSource DicomImage
        {
            get => _dicomImage;
            set
            {
                _dicomImage = value;
                OnPropertyChanged();
            }
        }

        /// <summary>
        /// Width of the visible intensity range. Bound to the Window slider in the UI.
        /// Triggers a window/level pass through the C++ DLL whenever changed.
        /// </summary>
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

        /// <summary>
        /// Center of the visible intensity range. Bound to the Level slider in the UI.
        /// Triggers a window/level pass through the C++ DLL whenever changed.
        /// </summary>
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

        /// <summary>
        /// Loads a DICOM file, normalises 12-bit data to the 16-bit range, and applies
        /// the initial window/level transform.
        /// </summary>
        /// <param name="imagePath">Absolute path to the .dcm file to load.</param>
        protected void LoadImage(string imagePath = null)
        {
            if (imagePath != null)
            {
                _dicomImageData = _dicomImageLoader.LoadImage(imagePath);
                PixelFormat _pixelFormat = (_dicomImageData.BitFormat == 8) ? PixelFormats.Gray8 : PixelFormats.Gray16;
                int bytesPerPixel = (_pixelFormat == PixelFormats.Gray8) ? 1 : 2;

                // 12-bit DICOM stores values in the lower 12 bits of each 16-bit word.
                // Left-shifting by 4 maps the full 12-bit range into the 16-bit display range
                // so that window/level arithmetic can treat all images uniformly.
                if (_dicomImageData.BitFormat == 12)
                {
                    for (int i = 0; i < _dicomImageData.ImageData.Length; i++)
                    {
                        _dicomImageData.ImageData[i] = (ushort)(_dicomImageData.ImageData[i] << 4);
                    }
                }

                // Keep an unmodified copy so window/level adjustments are always applied
                // to the original data rather than accumulating rounding errors.
                _originalImageData = (ushort[])_dicomImageData.ImageData.Clone();
                ApplyWindowLevel();
            }
        }

        private BitmapSource _dicomImage;
        private DicomImageLoader _dicomImageLoader;
        private DicomImageData _dicomImageData;
        private ushort[] _originalImageData;

        // Default window/level values tuned for a typical brain MRI.
        private int _windowValue = 34400;
        private int _levelValue = 17200;

        /// <summary>
        /// Clones the original pixel data, runs the C++ window/level transform on the copy,
        /// and rebuilds the BitmapSource that the UI is bound to.
        /// </summary>
        private void ApplyWindowLevel()
        {
            if (_originalImageData == null) return;

            // Clone so the native call modifies a temporary buffer, not the source data.
            ushort[] workingCopy = (ushort[])_originalImageData.Clone();

            ImageProcessingInterop.AdjustWindowLevel(workingCopy, _dicomImageData.Height, _dicomImageData.Width, _windowValue, _levelValue);

            PixelFormat pixelFormat = (_dicomImageData.BitFormat == 8) ? PixelFormats.Gray8 : PixelFormats.Gray16;
            int bytesPerPixel = (pixelFormat == PixelFormats.Gray8) ? 1 : 2;

            DicomImage = BitmapSource.Create(
                _dicomImageData.Width,
                _dicomImageData.Height,
                96, 96,
                pixelFormat,
                null,
                workingCopy,
                _dicomImageData.Width * bytesPerPixel);
        }
    }
}
