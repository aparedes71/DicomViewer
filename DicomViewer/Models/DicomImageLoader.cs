using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FellowOakDicom;
using FellowOakDicom.Imaging;

namespace DicomViewer.Models
{
    /// <summary>
    /// Holds the raw pixel data and metadata extracted from a DICOM file.
    /// </summary>
    public class DicomImageData
    {
        /// <summary>Raw 16-bit pixel values in row-major order.</summary>
        public ushort[] ImageData { get; set; }

        public int Width { get; set; }
        public int Height { get; set; }

        /// <summary>
        /// Bits stored per pixel as reported by the DICOM tag (0028,0101).
        /// Common values are 8, 12, and 16.
        /// </summary>
        public int BitFormat { get; set; }
    }

    /// <summary>
    /// Reads a DICOM file and extracts the first frame's pixel data using fo-dicom.
    /// </summary>
    public class DicomImageLoader
    {
        /// <summary>
        /// Opens a DICOM file and returns image data for the first frame.
        /// </summary>
        /// <param name="filePath">Absolute path to the .dcm file.</param>
        /// <returns>
        /// A <see cref="DicomImageData"/> instance containing pixel values and image dimensions.
        /// Pixel data is returned as a <c>ushort[]</c> regardless of the source bit depth.
        /// </returns>
        public DicomImageData LoadImage(string filePath)
        {
            var file = DicomFile.Open(filePath);
            var dataset = file.Dataset;

            var imageData = new DicomImageData();
            imageData.Width = dataset.GetSingleValue<int>(DicomTag.Columns);
            imageData.Height = dataset.GetSingleValue<int>(DicomTag.Rows);
            imageData.BitFormat = dataset.GetSingleValue<int>(DicomTag.BitsStored);

            var pixelData = DicomPixelData.Create(dataset);

            // fo-dicom returns raw bytes; reinterpret as 16-bit words via BlockCopy
            // to avoid per-element conversion overhead.
            byte[] rawBytes = pixelData.GetFrame(0).Data;
            ushort[] pixelArray = new ushort[rawBytes.Length / 2];
            Buffer.BlockCopy(rawBytes, 0, pixelArray, 0, rawBytes.Length);
            imageData.ImageData = pixelArray;

            return imageData;
        }
    }
}
