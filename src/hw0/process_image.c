#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // TODO Fill this in
    x = MIN(MAX(0, x), im.w - 1);
    y = MIN(MAX(0, y), im.h - 1);
    c = MIN(MAX(0, c), im.c - 1);
    return im.data[x + im.w * (y + im.h * c)];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    // TODO Fill this in
    if (x >= 0 && x < im.w && y >= 0 && y < im.h && c >= 0 && c < im.c) {
        im.data[x + im.w * (y + im.h * c)] = v;
    }
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // TODO Fill this in
    memcpy(copy.data, im.data, im.w * im.h * im.c * sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // TODO Fill this in
    for (int i = 0; i < gray.w; i++) {
        for (int j = 0; j < gray.h; j++) {
            float gray_val = 0.299 * get_pixel(im, i, j, 0) + 0.587 * get_pixel(im, i, j, 1) + 0.114 * get_pixel(im, i, j, 2);
            set_pixel(gray, i, j, 0, gray_val);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
    assert(c >= 0 && c < im.c);
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            float shifted = get_pixel(im, i, j, c) + v;
            set_pixel(im, i, j, c, shifted);
        }
    }
}

void clamp_image(image im)
{
    // TODO Fill this in
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            for (int k = 0; k < im.c; k++) {
                float shifted = MIN(MAX(0, get_pixel(im, i, j, k)), 1);
                set_pixel(im, i, j, k, shifted);
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in
    assert(im.c == 3);
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            float red = get_pixel(im, i, j, 0);
            float green = get_pixel(im, i, j, 1);
            float blue = get_pixel(im, i, j, 2);
            float value = three_way_max(red, green, blue);
            float C = value - three_way_min(red, green, blue);
            float saturation = value == 0 ? 0 : C / value;
            float hue = -INFINITY;
            if (C == 0) {
                hue = 0;
            } else if (value == red) {
                hue = (green - blue) / C;
            } else if (value == green) {
                hue = (blue - red) / C + 2;
            } else if (value == blue) {
                hue = (red - green) / C + 4;
            } else {
                // Should never get here
                printf("Something went wrong converting RGB to HSV");
            }

            hue = hue / 6 + (hue < 0 ? 1 : 0);
            set_pixel(im, i, j, 0, hue);
            set_pixel(im, i, j, 1, saturation);
            set_pixel(im, i, j, 2, value);
        }
    }
}

void hsv_to_rgb(image im)
{
    // TODO Fill this in
    assert(im.c == 3);
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            float hue = get_pixel(im, i, j, 0) * 6;
            float saturation = get_pixel(im, i, j, 1);
            float value = get_pixel(im, i, j, 2);
            float C = saturation * value;
            float m = value - C;
            float red, green, blue;
            float X = C * (1 - fabs(fmod(hue, 2) - 1));
            if (0 <= hue && hue <= 1) {
                red = C;
                green = X;
                blue = 0;
            } else if (1 < hue && hue <= 2) {
                red = X;
                green = C;
                blue = 0;
            } else if (2 < hue && hue <= 3) {
                red = 0;
                green = C;
                blue = X;
            } else if (3 < hue && hue <= 4) {
                red = 0;
                green = X;
                blue = C;
            } else if (4 < hue && hue <= 5) {
                red = X;
                green = 0;
                blue = C;
            } else if (5 < hue && hue <= 6) {
                red = C;
                green = 0;
                blue = X;
            } else {
                red = green = blue = -INFINITY;
                printf("Something went wrong converting HSV to RGB");
            }

            set_pixel(im, i, j, 0, red + m);
            set_pixel(im, i, j, 1, green + m);
            set_pixel(im, i, j, 2, blue + m);
        }
    }
}

void scale_image(image im, int c, float v) 
{
    assert(im.c == 3);
    assert(c >= 0 && c < im.c);
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            set_pixel(im, i, j, c, get_pixel(im, i, j, c) * v);
        }
    }
}
