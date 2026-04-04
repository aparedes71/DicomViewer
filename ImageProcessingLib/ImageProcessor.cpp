#include "pch.h"
#include "ImageProcessor.h"


extern "C" {
    __declspec(dllexport) void AdjustBrightness(unsigned char* pixel_data, int img_height, int img_width, int brightness_adjustment)
    {
        //Assuming grayscale image
        cv::Mat img = cv::Mat(img_width, img_height, CV_8UC1, pixel_data).clone(); //cloning to ensure if pixel_data is destroyed at some point the mat will not become invalid
        cv::Mat result;
       
        //cv add will handle clamping values such that they cannot go below 0 or 255 for 8 bit values
        cv::add(img, cv::Scalar(brightness_adjustment), result);

        if (img.isContinuous())
        {
            size_t img_size_bytes = img.total() * img.elemSize();
            std::memcpy(pixel_data, result.data, img_size_bytes);
        }

    }
}