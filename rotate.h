#pragma once
#include <array>

namespace RT {

typedef std::array<unsigned char, 4> Pixel;

void setDefaultPixel(Pixel);

/**
* Rotate image.
* Memory allocation for the destination image is managed inside.
*/
void rotate(unsigned char* srcImg, int srcWidth, int srcHeight,
            double theta, unsigned char*& dstImg,
            int& dstWidth, int& dstHeight, int comp = 4);

}