#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image/stb_image_write.h" 

int main(int argc, char* argv[]) {
    int testWidth = 100, testHeight = 100;
    uint8_t* testData;

    testData = (uint8_t*)calloc(testWidth*testHeight*3, 1);

    for (int y = 0; y < testHeight; y++) {
        for (int x = 0; x < testWidth; x++) {
            int pos = (y*testWidth + x) * 3;

            *(testData + pos + 0) = 255;
            *(testData + pos + 1) = 255;
            *(testData + pos + 2) = 255;
        }
    }

    stbi_write_png("test.png", testWidth, testHeight, 3, testData, testWidth*3);

    free(testData);

    return 0;
}