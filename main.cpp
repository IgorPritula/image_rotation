#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rdParty/stb_image.h"
#include "3rdParty/stb_image_write.h"
#include "rotate.h"

struct RotateSettings 
{
    std::string src;
    std::string dst;
    double angle;
    RT::Pixel defPixel;
};

int main()
{
    std::vector<RotateSettings> images;
    images.push_back({"res/32bit.png", "res/32bit_rotated.png", 200, {0, 0, 0, 255}});
    images.push_back({"res/24bit.png", "res/24bit_rotated.png", 45, {255, 255, 255, 255}});
    images.push_back({"res/transparency.png", "res/transparency_rotated.png", 160, {0, 0, 0, 0}});

    for (const auto& image : images) 
    {
        int width, height, comp;
        unsigned char* srcImg = stbi_load(image.src.c_str(), &width, &height, &comp, 0);
        if (!srcImg)
        {
            std::cout << "Failed to load image: " << image.src.c_str() << std::endl;
            continue;
        }

        unsigned char* dstImg = nullptr;
        int dstWidth, dstHeight;
        RT::setDefaultPixel(image.defPixel);
        RT::rotate(srcImg, width, height, image.angle, dstImg, dstWidth, dstHeight, comp);

        if (!dstImg)
        {
            std::cout << "Failed to rotate image: " << image.src.c_str() << std::endl;
            stbi_image_free(srcImg);
            continue;
        }

        stbi_write_png(image.dst.c_str(), dstWidth, dstHeight, comp, dstImg, comp*dstWidth);

        stbi_image_free(srcImg);
        delete[] dstImg;
    }

    std::cout << "Rotation completed successfully." << std::endl;
    return 0;
}
