#pragma once
#include <opencv2/core.hpp>

/*
 * Exported image processing functions called from the C# application via P/Invoke.
 * All functions operate in-place on a flat array of 16-bit grayscale pixel values.
 * The extern "C" linkage prevents C++ name mangling so the DLL exports match
 * the names declared in ImageProcessingInterop.cs.
 */
extern "C" {

    /*
     * Shifts every pixel by brightness_adjustment, clamping to [0, 65535].
     * Positive values brighten the image; negative values darken it.
     * Clamping is handled by OpenCV's cv::add, which saturates on overflow.
     *
     * pixel_data        - flat row-major array of 16-bit pixel values (modified in-place)
     * img_height        - image height in pixels
     * img_width         - image width in pixels
     * brightness_adjustment - signed offset to apply to each pixel
     */
    __declspec(dllexport) void AdjustBrightness(unsigned short* pixel_data, int img_height, int img_width, int brightness_adjustment);

    /*
     * Applies a standard radiology window/level contrast transform.
     *
     * The visible intensity range is [level - window/2, level + window/2].
     * Pixels below that range are clamped to 0 (black), pixels above are clamped
     * to 65535 (white), and pixels within the range are scaled linearly to [0, 65535].
     *
     * pixel_data - flat row-major array of 16-bit pixel values (modified in-place)
     * img_height - image height in pixels
     * img_width  - image width in pixels
     * window     - width of the visible intensity range
     * level      - center of the visible intensity range
     */
    __declspec(dllexport) void AdjustWindowLevel(unsigned short* pixel_data, int img_height, int img_width, int window, int level);
}
