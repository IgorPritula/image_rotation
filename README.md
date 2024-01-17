# image_rotation

Image rotation in multiple threads with bilinear interpolation.

Example:  
```
int width, height, comp;
unsigned char* srcImg = stbi_load("filename.png", &width, &height, &comp, 0);
unsigned char* dstImg = nullptr;
int dstWidth, dstHeight;

RT::rotate(srcImg, width, height, 45, dstImg, dstWidth, dstHeight, comp);

stbi_write_png("rotated_filename.png", dstWidth, dstHeight, comp, dstImg, comp*dstWidth);
stbi_image_free(srcImg);
delete[] dstImg;
```

![alt text](https://github.com/IgorPritula/image_rotation/blob/master/screenshot.png)
