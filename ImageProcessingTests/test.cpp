#include "pch.h"
#include "ImageProcessor.h"

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

