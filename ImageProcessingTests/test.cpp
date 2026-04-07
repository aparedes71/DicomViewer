#include "pch.h"
#include "ImageProcessor.h"

/*****************************************************************************************
BRIGHTNESS ADJUSTMENT TESTS
*****************************************************************************************/

// Brightness tests verify a simple per-pixel offset adjustment.
// A positive value is added to each pixel, negative subtracts.
// Values are clamped to the 0-255 range to prevent overflow/underflow.
// A brightness of zero should leave the image unchanged.
TEST(BrightnessTest, PositiveBrightnessIncreasesPixelValues)
{
    unsigned char test_img[] = { 100, 100, 100, 100 };
    AdjustBrightness(test_img, 2, 2, 50);

    EXPECT_EQ(test_img[0], 150);
    EXPECT_EQ(test_img[1], 150);
    EXPECT_EQ(test_img[2], 150);
    EXPECT_EQ(test_img[3], 150);
}
TEST(BrightnessTest, NegativeBrightnessDecreasesPixelValues)
{
    unsigned char test_img[] = { 100, 100, 100, 100 };
    AdjustBrightness(test_img, 2, 2, -50);

    EXPECT_EQ(test_img[0], 50);
    EXPECT_EQ(test_img[1], 50);
    EXPECT_EQ(test_img[2], 50);
    EXPECT_EQ(test_img[3], 50);

}
TEST(BrightnessTest, ClampsAt255WhenOverflow)
{
    unsigned char test_img[] = { 250, 250, 250, 240 };
    AdjustBrightness(test_img, 2, 2, 1000);

    EXPECT_EQ(test_img[0], 255);
    EXPECT_EQ(test_img[1], 255);
    EXPECT_EQ(test_img[2], 255);
    EXPECT_EQ(test_img[3], 255);

}
TEST(BrightnessTest, ClampsAt0WhenUnderflow)
{
    unsigned char test_img[] = { 10, 100, 90, 60 };
    AdjustBrightness(test_img, 2, 2, -1000);

    EXPECT_EQ(test_img[0], 0);
    EXPECT_EQ(test_img[1], 0);
    EXPECT_EQ(test_img[2], 0);
    EXPECT_EQ(test_img[3], 0);

}
TEST(BrightnessTest, ZeroBrightnessLeavesDataUnchanged)
{
    unsigned char test_img[] = { 10, 100, 90, 60 };
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
// Pixels below (level - window/2) clamp to black (0), above (level + window/2) clamp to white (255),
// and pixels within the range are scaled proportionally to 0-255.
// Note: integer rounding may cause values to be off by 1 from naive expectations (e.g., 126 instead of 127).
TEST(WindowLevelTest, PixelBelowMinClampsToZero)
{
    unsigned char test_img[] = { 10, 20, 30, 40 };
    AdjustWindowLevel(test_img, 2, 2, 100, 200);
    EXPECT_EQ(test_img[0], 0);
    EXPECT_EQ(test_img[1], 0);
    EXPECT_EQ(test_img[2], 0);
    EXPECT_EQ(test_img[3], 0);
}

TEST(WindowLevelTest, PixelAboveMaxClampsTo255)
{
    unsigned char test_img[] = { 200, 210, 220, 255 };
    AdjustWindowLevel(test_img, 2, 2, 100, 100);
    EXPECT_EQ(test_img[0], 255);
    EXPECT_EQ(test_img[1], 255);
    EXPECT_EQ(test_img[2], 255);
    EXPECT_EQ(test_img[3], 255);
}

TEST(WindowLevelTest, PixelAtMidpointMapsTo127)
{
    unsigned char test_img[] = { 100, 100, 100, 100 };
    AdjustWindowLevel(test_img, 2, 2, 100, 100);
    EXPECT_EQ(test_img[0], 127);
}

TEST(WindowLevelTest, PixelsScaleProportionallyWithinRange)
{
    unsigned char test_img[] = { 50, 75, 100, 150 };
    AdjustWindowLevel(test_img, 2, 2, 100, 100);
    EXPECT_EQ(test_img[0], 0);
    EXPECT_EQ(test_img[1], 63);
    EXPECT_EQ(test_img[2], 127);
    EXPECT_EQ(test_img[3], 255);
}

TEST(WindowLevelTest, WideWindowPreservesValues)
{
    unsigned char test_img[] = { 0, 127, 255, 50 };
    AdjustWindowLevel(test_img, 2, 2, 256, 128);
    EXPECT_EQ(test_img[0], 0);
    EXPECT_EQ(test_img[1], 126);
    EXPECT_EQ(test_img[2], 254);
    EXPECT_EQ(test_img[3], 49);
}

TEST(WindowLevelTest, NarrowWindowCrushesPixels)
{
    unsigned char test_img[] = { 0, 100, 128, 255 };
    AdjustWindowLevel(test_img, 2, 2, 10, 128);
    EXPECT_EQ(test_img[0], 0);
    EXPECT_EQ(test_img[1], 0);
    EXPECT_EQ(test_img[2], 127);
    EXPECT_EQ(test_img[3], 255);
}

