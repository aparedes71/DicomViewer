using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace DicomViewer.Models
{
    static class ImageProcessingInterop
    {
        [DllImport("ImageProcessingLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AdjustBrightness(ushort[] pixelData, int imgHeight, int imgWidth, int brightnessValue);

        [DllImport("ImageProcessingLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AdjustWindowLevel(ushort[] pixelData, int imgHeight, int imgWidth, int window,int level);
    }
}
