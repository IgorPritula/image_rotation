#include "rotate.h"
#include <math.h>
#include <algorithm>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

#define PI 3.14159265

using namespace RT;

namespace 
{

class RTimer 
{
public:
    RTimer()
    {
        mStartTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    }

    ~RTimer()
    {
        auto endTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        auto delta = endTimestamp - mStartTimestamp;
        std::cout << "Time: " << delta.count() << " ms\n";
    }

private:
    std::chrono::milliseconds mStartTimestamp;
};

struct RotationData 
{
    unsigned char* srcImg; 
    int srcWidth, srcHeight;
    unsigned char* dstImg;
    int dstWidth, dstHeight;
    float sin, cos; 
    int comp;
};

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

void rotationJob(RotationData& rd, int start, int end)
{
    int dstHalfWidth = rd.dstWidth / 2;
    int dstHalfHeight = rd.dstHeight / 2;
    for (int y = start; y < end; ++y) {
        for (int x = 0; x < rd.dstWidth; ++x) {

            float srcX = static_cast<float>(rd.cos*((float)(x)-dstHalfWidth) + rd.sin*((float)(y)-dstHalfHeight) + (float)(rd.srcWidth / 2));
            float srcY = static_cast<float>(-rd.sin*((float)(x)-dstHalfWidth) + rd.cos*((float)(y)-dstHalfHeight) + (float)(rd.srcHeight / 2));

            // Skip pixel processing which definitely out of bounds.
            bool isPixelValid = (-1 < srcY && srcY <= rd.srcHeight) && (-1 < srcX && srcX <= rd.srcWidth);
            Pixel px = isPixelValid ? getBilinearPixel(rd.srcImg, rd.srcWidth, rd.srcHeight, srcX, srcY, rd.comp) : defPixel;
            
            int dstIndex = (rd.dstWidth * y * rd.comp) + (x * rd.comp);
            memcpy(&rd.dstImg[dstIndex], &px, rd.comp);
        }
    }
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
    //RTimer timer;
    double rads = theta * (PI / 180);
    float cs = cos(rads);
    float ss = sin(rads);

    // Calculate new image dimensions
    dstWidth = round(fabs(srcWidth * cs) + fabs(srcHeight * ss));
    dstHeight = round(fabs(srcHeight * cs) + fabs(srcWidth * ss));

    dstImg = new unsigned char[dstHeight * dstWidth * comp];
    if (!dstImg)
        return;

    RotationData rd {srcImg, srcWidth, srcHeight, dstImg, dstWidth, dstHeight, ss, cs, comp};

    // Perform rotation in multipal threads for large images.
    if (dstHeight > 256)
    {
        int trdCount = 8;
        int rowsPerThread = dstHeight/trdCount;
        std::vector<std::thread> threads {};

        for (int i = 0; i < trdCount; i++)
        {
            int start = rowsPerThread * i;
            int end = i+1 < trdCount ? rowsPerThread * (i+1) : dstHeight;
            threads.push_back(std::thread(rotationJob, std::ref(rd), start, end));
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
    }
    else
    {
        rotationJob(rd, 0, dstHeight);
    }

}

}