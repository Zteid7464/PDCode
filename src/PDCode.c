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
void fillRect(int x, int y, int width, int height, uint8_t* img, int img_width, int img_height, uint8_t* color, unsigned int bpp);

// will write a Point Distance Code of data into the given image. Will return 0 on success and non zero on non succss
int writePDC(int point_size, unsigned int distance_count, unsigned int* distances, unsigned int data_size, uint8_t* data, uint8_t* img, int img_width, int img_height);

// will write the data encoded in a given point disance image into data and will wirte the size of data into data_size. Returns 0 on succes and non zero on non success 
int readPDC(int point_size, unsigned int distance_count, unsigned int* distances, uint8_t* data, unsigned int* data_size, uint8_t* img, int img_width, int img_height);

int main(int argc, char* argv[]) {
    int testWidth = 1000, testHeight = 1000;
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

    unsigned int distances[] = {7, 17};
    uint8_t* data = "Hello world!\n How are you doing Today? Wie geht es dir? Mir geht es gut! Der Test war erfolgreich!\0";

    writePDC(10, 2, distances, 100, data, testData, testWidth, testHeight);

    stbi_write_png("test.png", testWidth, testHeight, 3, testData, testWidth*3);

    free(testData);

    return 0;
}

int xyToPixelCoords(int x, int y, int img_width, int bpp) {
    return (y*img_width + x) * bpp;
}

void fillRect(int x, int y, int width, int height, uint8_t* img, int img_width, int img_height, uint8_t* color, unsigned int bpp) {
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

int writePDC(int point_size, unsigned int distance_count, unsigned int* distances, unsigned int data_size, uint8_t* data,  uint8_t* img, int img_width, int img_height) {
    int currentX = 0;
    int currentY = 0;
    uint8_t current_color[] = {0, 0, 0};

    // draw the reference point
    fillRect(currentX, currentY, point_size, point_size, img, img_width, img_height, current_color, 3);
    
    // loop thru each bit of the data and TODO: make it compatible with any number of distances
    for (unsigned int dbyte = 0; dbyte < data_size; dbyte++) {
        for (int dbit = 7; dbit >= 0; dbit--) {
            // the atual part of the data that we access 
            unsigned int part = (*(data + dbyte) >> dbit) & 1;
            printf("%u", part);

            // update the distances with the right disance. Plus point size becouse the distance starts at the end of the point 
            currentX += *(distances + part) + point_size;
            // check if we have to jump to a new line and reset x
            if (currentX + point_size > img_width) {
                currentY += point_size;
                // no plus point size becouse we start at the edge of the image
                currentX = *(distances + part);
            }
            // fill the rectangle 
            fillRect(currentX, currentY, point_size, point_size, img, img_width, img_height, current_color, 3);
        }
        printf(" %d %u\n", *(data + dbyte), dbyte);
    }

    return 0;
}

int readPDC(int point_size, unsigned int distance_count, unsigned int* distances, uint8_t* data, unsigned int* data_size, uint8_t* img, int img_width, int img_height) {
    // we know that the first point is going to be black so we can skipp. start in the middel of the point 
    int readX = point_size, readY = point_size / 2;
    int current_distance = 0;
    _Bool was_white = 0;
    _Bool was_black = 0;

    // messure the distance
    // TODO make it compare to all colors. And make it look for black enouth not just black
    while (1)  {
        current_distance ++;

        readY ++;

        // check if the pixel is white. Do that becouse otherwise we could se the point that we left
        // TODO make it check all values and look for white enouth not just white
        if (*(img + xyToPixelCoords(readX, readY, img_width, 3)) == 255)
            was_white = 1;
        else if (*(img + xyToPixelCoords(readX, readY, img_width, 3)) == 0)
            was_black = 1;
    } ;

}