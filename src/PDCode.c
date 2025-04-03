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
    uint8_t* testImg;

    testImg = (uint8_t*)calloc(testWidth*testHeight*3, 1);

    for (int y = 0; y < testHeight; y++) {
        for (int x = 0; x < testWidth; x++) {
            int pos = (y*testWidth + x) * 3;

            *(testImg + pos + 0) = 255;
            *(testImg + pos + 1) = 255;
            *(testImg + pos + 2) = 255;
        
        }
    }

    unsigned int distances[] = {7, 17};
    uint8_t* data = "Hello, world!\0";

    writePDC(10, 2, distances, 15, data, testImg, testWidth, testHeight);

    stbi_write_png("test.png", testWidth, testHeight, 3, testImg, testWidth*3);

    uint8_t* recv_data = NULL;
    unsigned int recv_data_size = 0;

    readPDC(10, 2, distances, recv_data, &recv_data_size, testImg, testWidth, testHeight);

    printf("Image contains %d bytes of data\n", recv_data_size);

    free(testImg);

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

    // write the end 
    fillRect(currentX, currentY, point_size*3, point_size, img, img_width, img_height, current_color, 3);

    return 0;
}

int readPDC(int point_size, unsigned int distance_count, unsigned int* distances, uint8_t* data, unsigned int* data_size, uint8_t* img, int img_width, int img_height) {
    int readX = 0, readY = point_size / 2;
    int messured_distance = 0;
    int threshold = 50;
    int last_color = 0;
    int current_color = 0;

    uint8_t current_byte = 0;


    // messure the distance
    for (int interartions = 0;;)  {
        messured_distance++;

        readX ++;

        // check if we have to jump to a new line and reset x
        if (readX + point_size > img_width) {
            readY += point_size;
            // no plus point size becouse we start at the edge of the image
            readX = 0;
            // also reset the distance
            messured_distance = 0;
            printf("newline\n");
        }

        // check what color the pixel has
        // TODO make it check all values
        if (*(img + xyToPixelCoords(readX, readY, img_width, 3)) >= 255 - threshold)
            current_color = 1;
        else if (*(img + xyToPixelCoords(readX, readY, img_width, 3)) <= threshold)
            current_color = 0;

        // if the last color was white and now its black we have a distance!
        if (last_color == 1 && current_color == 0) {
            messured_distance = messured_distance - point_size;
            (*data_size)++;

            // only increase interations when wehre actulay reading somthing 
            interartions++;

            unsigned int distance = 0;

            // figure out what distance it should be
            // set best distance diference to a big ass value so everything else is smaller 
            for (int i = 0, bestdd = 2147483647; i < distance_count; i++) {
                // compare the messured distance to every distance that we have and chose the one thats nearest
                if (abs(messured_distance - (int)*(distances + i)) < bestdd) {
                    bestdd = abs(messured_distance - (int)*(distances + i));
                    distance = i;
                }
            }
            printf("messured: %d closest: %u\n", messured_distance, distance);
            
            if (interartions % 8 == 0) {
                printf("byte: %d i: %d\n", current_byte, interartions);
                current_byte = 0;
            }

            // update the current byte TODO update for more than binary
            current_byte = (current_byte << 1) | distance;

            messured_distance = 0;
        }

        // A point distance code is terminated when a black point biger than 2 times point size is there then we reached the end
        if (messured_distance >= 2*point_size && current_color == 0 && last_color == 0) 
            break;

        last_color = current_color;
    };

    // data size is in bytes but we saved it in bits
    *data_size = *data_size/8;

    return 0;
}