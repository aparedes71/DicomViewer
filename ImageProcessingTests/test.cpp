#include "pch.h"
#include "ImageProcessor.h"

namespace ImageProcessingTests
{
    /*****************************************************************************************
    BRIGHTNESS ADJUSTMENT TESTS
    *****************************************************************************************/

    // Brightness tests verify a simple per-pixel offset adjustment.
    // A positive value is added to each pixel, negative subtracts.
    // Values are clamped to the 0-65535 range to prevent overflow/underflow.
    // A brightness of zero should leave the image unchanged.
    TEST(BrightnessTest, PositiveBrightnessIncreasesPixelValues)
    {
        ushort test_img[] = { 100, 100, 100, 100 };
        AdjustBrightness(test_img, 2, 2, 50);

        EXPECT_EQ(test_img[0], 150);
        EXPECT_EQ(test_img[1], 150);
        EXPECT_EQ(test_img[2], 150);
        EXPECT_EQ(test_img[3], 150);
    }
    TEST(BrightnessTest, NegativeBrightnessDecreasesPixelValues)
    {
        ushort test_img[] = { 100, 100, 100, 100 };
        AdjustBrightness(test_img, 2, 2, -50);

        EXPECT_EQ(test_img[0], 50);
        EXPECT_EQ(test_img[1], 50);
        EXPECT_EQ(test_img[2], 50);
        EXPECT_EQ(test_img[3], 50);

    }
    TEST(BrightnessTest, ClampsAt65535WhenOverflow)
    {
        ushort test_img[] = { 65000, 65000, 65000, 65000 };
        AdjustBrightness(test_img, 2, 2, 1000);

        EXPECT_EQ(test_img[0], 65535);
        EXPECT_EQ(test_img[1], 65535);
        EXPECT_EQ(test_img[2], 65535);
        EXPECT_EQ(test_img[3], 65535);

    }
    TEST(BrightnessTest, ClampsAt0WhenUnderflow)
    {
        ushort test_img[] = { 10, 100, 90, 60 };
        AdjustBrightness(test_img, 2, 2, -1000);

        EXPECT_EQ(test_img[0], 0);
        EXPECT_EQ(test_img[1], 0);
        EXPECT_EQ(test_img[2], 0);
        EXPECT_EQ(test_img[3], 0);

    }
    TEST(BrightnessTest, ZeroBrightnessLeavesDataUnchanged)
    {
        ushort test_img[] = { 10, 100, 90, 60 };
        AdjustBrightness(test_img, 2, 2, 0);

        EXPECT_EQ(test_img[0], 10);
        EXPECT_EQ(test_img[1], 100);
        EXPECT_EQ(test_img[2], 90);
        EXPECT_EQ(test_img[3], 60);

    }
    /*****************************************************************************************
    WINDOW LEVEL TESTS
    *****************************************************************************************/

    // Window/Level tests simulate the standard radiology brightness/contrast adjustment.
    // "Level" sets the center of the visible range, "Window" sets the width of that range.
    // Pixels below (level - window/2) clamp to black (0), above (level + window/2) clamp to white (65535),
    // and pixels within the range are scaled proportionally to 0-65535.
    // Note: integer rounding may cause values to be off by 1 from naive expectations.
    TEST(WindowLevelTest, PixelBelowMinClampsToZero)
    {
        ushort test_img[] = { 10, 20, 30, 40 };
        AdjustWindowLevel(test_img, 2, 2, 100, 200);
        EXPECT_EQ(test_img[0], 0);
        EXPECT_EQ(test_img[1], 0);
        EXPECT_EQ(test_img[2], 0);
        EXPECT_EQ(test_img[3], 0);
    }

    TEST(WindowLevelTest, PixelAboveMaxClampsTo65535)
    {
        ushort test_img[] = { 200, 210, 220, 65535 };
        AdjustWindowLevel(test_img, 2, 2, 100, 100);
        EXPECT_EQ(test_img[0], 65535);
        EXPECT_EQ(test_img[1], 65535);
        EXPECT_EQ(test_img[2], 65535);
        EXPECT_EQ(test_img[3], 65535);
    }

    TEST(WindowLevelTest, PixelAtMidpointMapsTo32767)
    {
        ushort test_img[] = { 100, 100, 100, 100 };
        AdjustWindowLevel(test_img, 2, 2, 100, 100);
        EXPECT_EQ(test_img[0], 32767);
    }

    TEST(WindowLevelTest, PixelsScaleProportionallyWithinRange)
    {
        ushort test_img[] = { 50, 75, 100, 150 };
        AdjustWindowLevel(test_img, 2, 2, 100, 100);
        EXPECT_EQ(test_img[0], 0);
        EXPECT_EQ(test_img[1], 16383);
        EXPECT_EQ(test_img[2], 32767);
        EXPECT_EQ(test_img[3], 65535);
    }

    TEST(WindowLevelTest, WideWindowPreservesValues)
    {
        ushort test_img[] = { 0, 127, 255, 50 };
        AdjustWindowLevel(test_img, 2, 2, 256, 128);
        EXPECT_EQ(test_img[0], 0);
        EXPECT_EQ(test_img[1], 32511);
        EXPECT_EQ(test_img[2], 65279);
        EXPECT_EQ(test_img[3], 12799);
    }

    TEST(WindowLevelTest, NarrowWindowCrushesPixels)
    {
        ushort test_img[] = { 0, 100, 128, 255 };
        AdjustWindowLevel(test_img, 2, 2, 10, 128);
        EXPECT_EQ(test_img[0], 0);
        EXPECT_EQ(test_img[1], 0);
        EXPECT_EQ(test_img[2], 32767);
        EXPECT_EQ(test_img[3], 65535);
    }
}