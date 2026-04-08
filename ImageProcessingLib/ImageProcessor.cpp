#include "pch.h"
#include "ImageProcessor.h"

extern "C" {

    __declspec(dllexport) void AdjustBrightness(unsigned short* pixel_data, int img_height, int img_width, int brightness_adjustment)
    {
        // Wrap the raw buffer in a Mat header without copying — the clone() call below
        // makes a deep copy so the Mat owns its memory independently of pixel_data.
        // This is necessary because cv::add writes to a separate output Mat, and we
        // need to copy the result back into the caller's buffer afterward.
        cv::Mat img = cv::Mat(img_width, img_height, CV_16UC1, pixel_data).clone();
        cv::Mat result;

        // cv::add saturates at the type's maximum, so values cannot wrap around or
        // exceed the 16-bit range. No manual clamping is required.
        cv::add(img, cv::Scalar(brightness_adjustment), result);

        if (img.isContinuous())
        {
            size_t img_size_bytes = img.total() * img.elemSize();
            std::memcpy(pixel_data, result.data, img_size_bytes);
        }
    }

    __declspec(dllexport) void AdjustWindowLevel(unsigned short* pixel_data, int img_height, int img_width, int window, int level)
    {
        // Compute the intensity bounds for the visible range.
        double min = level - (window / 2.0);
        double max = level + (window / 2.0);

        for (int pixel_idx = 0; pixel_idx < img_height * img_width; pixel_idx++)
        {
            unsigned short old_pixel = pixel_data[pixel_idx];

            if (old_pixel < min)
            {
                pixel_data[pixel_idx] = 0;
            }
            else if (old_pixel > max)
            {
                pixel_data[pixel_idx] = 65535;
            }
            else
            {
                // Scale the pixel linearly from its position within [min, max] to [0, 65535].
                pixel_data[pixel_idx] = static_cast<unsigned short>(
                    ((old_pixel - min) / (max - min)) * 65535
                );
            }
        }
    }

}
