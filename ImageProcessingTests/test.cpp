#include "pch.h"
#include "ImageProcessor.h"

namespace ImageProcessingTests
{
    /*
     * AdjustBrightness Tests
     *
     * These tests verify the per-pixel offset logic. A positive adjustment increases
     * each pixel value, negative decreases it, and the output must clamp to [0, 65535]
     * on overflow/underflow. An adjustment of zero is a no-op.
     */

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

    /*
     * AdjustWindowLevel Tests
     *
     * Window/level is the standard radiology contrast control. "Level" sets the
     * midpoint of the visible intensity range; "Window" sets its width. Pixels
     * outside the range are hard-clamped to black (0) or white (65535), while
     * pixels inside are scaled linearly to fill the full output range.
     *
     * Note: integer truncation means the midpoint maps to 32767, not 32768.
     */

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
        // A narrow window means most pixels fall outside the visible range and
        // get clamped. Only pixels very close to the level value are scaled.
        ushort test_img[] = { 0, 100, 128, 255 };
        AdjustWindowLevel(test_img, 2, 2, 10, 128);

        EXPECT_EQ(test_img[0], 0);
        EXPECT_EQ(test_img[1], 0);
        EXPECT_EQ(test_img[2], 32767);
        EXPECT_EQ(test_img[3], 65535);
    }
}
