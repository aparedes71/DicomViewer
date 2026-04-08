using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace DicomViewer.Models
{
    /// <summary>
    /// P/Invoke bindings for the native ImageProcessingLib DLL.
    /// The DLL must be present in the application's output directory.
    /// All functions operate in-place on the supplied pixel buffer.
    /// </summary>
    static class ImageProcessingInterop
    {
        /// <summary>
        /// Shifts every pixel value by <paramref name="brightnessValue"/>, clamping to [0, 65535].
        /// </summary>
        /// <param name="pixelData">Flat array of 16-bit pixel values in row-major order.</param>
        /// <param name="imgHeight">Image height in pixels.</param>
        /// <param name="imgWidth">Image width in pixels.</param>
        /// <param name="brightnessValue">Signed offset to apply. Positive brightens, negative darkens.</param>
        [DllImport("ImageProcessingLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AdjustBrightness(ushort[] pixelData, int imgHeight, int imgWidth, int brightnessValue);

        /// <summary>
        /// Applies a standard radiology window/level transform to the pixel buffer.
        /// Pixels below <c>level - window/2</c> map to 0; pixels above <c>level + window/2</c>
        /// map to 65535; pixels within the range are scaled linearly.
        /// </summary>
        /// <param name="pixelData">Flat array of 16-bit pixel values in row-major order.</param>
        /// <param name="imgHeight">Image height in pixels.</param>
        /// <param name="imgWidth">Image width in pixels.</param>
        /// <param name="window">Width of the visible intensity range.</param>
        /// <param name="level">Center of the visible intensity range.</param>
        [DllImport("ImageProcessingLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AdjustWindowLevel(ushort[] pixelData, int imgHeight, int imgWidth, int window, int level);
    }
}
