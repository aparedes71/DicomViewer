#include "pch.h"
#include "ImageProcessor.h"


extern "C" {
    __declspec(dllexport) void AdjustBrightness(unsigned short* pixel_data, int img_height, int img_width, int brightness_adjustment)
    {
        //Assuming grayscale image
        cv::Mat img = cv::Mat(img_width, img_height, CV_16UC1, pixel_data).clone(); //cloning to ensure if pixel_data is destroyed at some point the mat will not become invalid
        cv::Mat result;
       
        //cv add will handle clamping values such that they cannot go below 0 or 255 for 8 bit values
        cv::add(img, cv::Scalar(brightness_adjustment), result);

        if (img.isContinuous())
        {
            size_t img_size_bytes = img.total() * img.elemSize();
            std::memcpy(pixel_data, result.data, img_size_bytes);
        }

    }

    __declspec(dllexport) void AdjustWindowLevel(unsigned short* pixel_data, int img_height, int img_width, int window,int level) 
    {
        double min = level - (window / 2);
        double max = level + (window / 2);


        for (int pixel_idx = 0; pixel_idx < img_height*img_width; pixel_idx ++)
        {
            unsigned short old_pixel = pixel_data[pixel_idx];
            if (old_pixel < min)
            {
                pixel_data[pixel_idx] = 0;
            }
            else if(old_pixel > max)
            {
                //pixel_data[pixel_idx] = 255; //8 bit
                pixel_data[pixel_idx] = 65535; //16 bit
            }
            else 
            {
                pixel_data[pixel_idx] = ( 
                    (old_pixel - min) / (max - min) 
                    ) * 65535;
            }
            
        }
    }

}