#include "rotate.h"
#include <math.h>
#include <algorithm>

#define PI 3.14159265

using namespace RT;

namespace 
{

Pixel defPixel {0, 0, 0, 255};

Pixel getPixel(const unsigned char* srcImg, int width, int height, int x, int y, int comp)
{
    if (x >= width || y >= height || x < 0 || y < 0)
    {
        return defPixel;
    }
    
    Pixel px;
    memcpy(&px, &srcImg[(width * y * comp) + (x * comp)], comp);
    return px;
}

Pixel getBilinearPixel(const unsigned char* srcImg, int width, int height, float x, float y, int comp)
{
    int xFloor = static_cast<int>(floor(x));
    int yFloor = static_cast<int>(floor(y));
    int xCeil = static_cast<int>(ceil(x));
    int yCeil = static_cast<int>(ceil(y));

    Pixel px = defPixel;

    double dx = x - xFloor;
    double dy = y - yFloor;

    Pixel ffPixel = getPixel(srcImg, width, height, xFloor, yFloor, comp);
    Pixel cfPixel = getPixel(srcImg, width, height, xCeil,  yFloor, comp);
    Pixel fcPixel = getPixel(srcImg, width, height, xFloor, yCeil,  comp);
    Pixel ccPixel = getPixel(srcImg, width, height, xCeil,  yCeil,  comp);

    for (int i = 0; i < comp; i++) 
    {
        unsigned char value = static_cast<unsigned char>(
            (1 - dx) * (1 - dy) * ffPixel[i] +
            dx * (1 - dy) * cfPixel[i] +
            (1 - dx) * dy * fcPixel[i] +
            dx * dy * ccPixel[i]
        );

        px[i] = value;
    }

    return px;
}

}


namespace RT 
{

void setDefaultPixel(Pixel px)
{
    defPixel = px;
}

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
    for (int y = 0; y < dstHeight; ++y) {
        for (int x = 0; x < dstWidth; ++x) {

            float srcX = static_cast<float>(cs*((float)(x)-dstHalfWidth) + ss*((float)(y)-dstHalfHeight) + xCenter);
            float srcY = static_cast<float>(-ss*((float)(x)-dstHalfWidth) + cs*((float)(y)-dstHalfHeight) + yCenter);

            // Skip pixel processing which definitely out of bounds.
            bool isPixelValid = (-1 < srcY && srcY <= srcHeight) && (-1 < srcX && srcX <= srcWidth);
            Pixel px = isPixelValid ? getBilinearPixel(srcImg, srcWidth, srcHeight, srcX, srcY, comp) : defPixel;
            
            int dstIndex = (dstWidth * y * comp) + (x * comp);
            memcpy(&dstImg[dstIndex], &px, comp);
        }
    }

}

}