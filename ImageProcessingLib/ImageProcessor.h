#pragma once
#include<opencv2/core.hpp>	

extern "C" {
    __declspec(dllexport) void AdjustBrightness(unsigned char* pixel_data , int img_height, int img_width, int brightness_adjustment);
}