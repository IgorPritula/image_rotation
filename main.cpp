#include <iostream>
#include <math.h>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rdParty/stb_image.h"
#include "3rdParty/stb_image_write.h"

#define PI 3.14159265

void rotate(unsigned char* srcImg, int srcWidth, int srcHeight,
            double theta, unsigned char*& dstImg,
            int& dstWidth, int& dstHeight, int comp = 4);

int main()
{
    const char* img_source = "res/32bit.png";
    const double angle = 200;

    int width, height, comp;
    unsigned char* srcImg = stbi_load(img_source, &width, &height, &comp, 0);
    if (!srcImg)
    {
        std::cout << "Failed to load image: " << img_source << std::endl;
        return 1;
    }

    unsigned char* dstImg = nullptr;
    int widthR, heightR;
    rotate(srcImg, width, height, angle, dstImg, widthR, heightR, comp);

    if (!dstImg)
    {
        std::cout << "Failed to rotate image" << std::endl;
        return 2;
    }

    stbi_write_png( "res/32bit_rotated.png", widthR, heightR, comp, dstImg, comp*widthR);

    stbi_image_free(srcImg);
    delete[] dstImg;

    std::cout << "Rotation completed successfully." << std::endl;
    return 0;
}

/**
* Rotate image.
* Memory allocation for the destination image is managed inside.
*/
void rotate(unsigned char* srcImg, int srcWidth, int srcHeight,
            double theta, unsigned char*& dstImg,
            int& dstWidth, int& dstHeight, int comp)
{
    double rads = theta * (PI / 180);
    float cs = cos(rads);
    float ss = sin(rads);

    float xCenter = (float)(srcWidth)/2.0;
    float yCenter = (float)(srcHeight)/2.0;

    // Calculate new image dimensions
    dstWidth = ceil(fabs(srcWidth * cs) + fabs(srcHeight * ss));
    dstHeight = ceil(fabs(srcHeight * cs) + fabs(srcWidth * ss));

    dstImg = new unsigned char[dstHeight * dstWidth * comp];
    if (!dstImg)
        return;

    int dstHalfWidth = dstWidth / 2;
    int dstHalfHeight = dstHeight / 2;
    unsigned char defPixel[] = {0, 0, 0, 255};
    for (int y = 0; y < dstHeight; ++y) {
        for (int x = 0; x < dstWidth; ++x) {

            int srcX = static_cast<int>(cs*((float)(x)-dstHalfWidth) + ss*((float)(y)-dstHalfHeight) + xCenter);
            int srcY = static_cast<int>(-ss*((float)(x)-dstHalfWidth) + cs*((float)(y)-dstHalfHeight) + yCenter);

            int dstIndex = (dstWidth * y * comp) + (x * comp);
            if (srcY >= 0 && srcY < srcHeight && srcX >= 0 && srcX < srcWidth)
            {
                int srcIndex = (srcWidth * srcY * comp) + (srcX * comp);
                memcpy(&dstImg[dstIndex], &srcImg[srcIndex], comp);
            }
            else 
            {
                memcpy(&dstImg[dstIndex], &defPixel, comp);
            }
        }
    }

}
