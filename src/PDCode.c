#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image/stb_image_write.h" 

// Will return the one dimesional pixel coordinate from the given x y coordinates, the width of the image and the bytes per pixel 
int xyToPixelCoords(int x, int y, int imgWidth, int bpp);
// will fill a rectangle of given size in given color on the given image  
void fillRect(int x, int y, int width, int height, uint8_t* img, int img_width, int img_height, int* color, int bpp);

int main(int argc, char* argv[]) {
    int testWidth = 100, testHeight = 100;
    uint8_t* testData;

    testData = (uint8_t*)calloc(testWidth*testHeight*3, 1);

    for (int y = 0; y < testHeight; y++) {
        for (int x = 0; x < testWidth; x++) {
            int pos = (y*testWidth + x) * 3;

            if (x % 2 == 0) {
                *(testData + pos + 0) = 255;
                *(testData + pos + 1) = 255;
                *(testData + pos + 2) = 255;
            } else {
                *(testData + pos + 0) = 255;
                *(testData + pos + 1) = 0;
                *(testData + pos + 2) = 0;
            }
        }
    }

    int rectColor[] = {0, 0, 255};
    fillRect(5, 10, 15, 15, testData, testWidth, testWidth, rectColor, 3);

    stbi_write_png("test.png", testWidth, testHeight, 3, testData, testWidth*3);

    free(testData);

    return 0;
}

int xyToPixelCoords(int x, int y, int img_width, int bpp) {
    return (y*img_width + x) * bpp;
}

void fillRect(int x, int y, int width, int height, uint8_t* img, int img_width, int img_height, int* color, int bpp) {
    // loop thru the pixels 
    for (int yPos = y; yPos <= y + height; ++yPos) {
        for (int xPos = x; xPos <= x + width; ++xPos) {
            int pixelPos = xyToPixelCoords(xPos, yPos, img_width, bpp);
            
            // only draw the pixel if it's inside the image
            if (xPos >= 0 && yPos >= 0 && xPos < img_width && yPos < img_height) {
                *(img + pixelPos + 0) = *(color);
                *(img + pixelPos + 1) = *(color+1);
                *(img + pixelPos + 2) = *(color+2);
            }
        }   
    }   
}