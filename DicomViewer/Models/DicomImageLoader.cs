using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FellowOakDicom;
using FellowOakDicom.Imaging;

namespace DicomViewer.Models
{
    public class DicomImageData
    {   public ushort[] ImageData { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }
        public int BitFormat { get; set; } // 8bit,12bit,16bit, etc

    }

    public class DicomImageLoader
    {
        public DicomImageData LoadImage(string filePath)
        {
            var file = DicomFile.Open(filePath);
            var dataset = file.Dataset;

            var imageData = new DicomImageData();
            imageData.Width = dataset.GetSingleValue<int>(DicomTag.Columns);
            imageData.Height = dataset.GetSingleValue<int>(DicomTag.Rows);
            imageData.BitFormat = dataset.GetSingleValue<int>(DicomTag.BitsStored);

            var pixelData = DicomPixelData.Create(dataset);
            byte[] rawBytes = pixelData.GetFrame(0).Data;
            ushort[] pixelArray = new ushort[rawBytes.Length / 2];
            Buffer.BlockCopy(rawBytes, 0, pixelArray, 0, rawBytes.Length);
            imageData.ImageData = pixelArray;

            //imageData.ImageData = pixelData.GetFrame(0).Data;

            return imageData;
        }
        //var image = new DicomImage(@"C:\Users\AndrewParedes\source\repos\DicomViewer\SampleData\MRBRAIN.DCM");   
    }

    
}
