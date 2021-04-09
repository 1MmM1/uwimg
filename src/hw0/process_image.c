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

// Source: https://www.easyrgb.com/en/math.php
void rgb_to_hcl(image im) {
    float red, green, blue;
    float var_R, var_G, var_B;
    float var_U, var_V, var_Y;
    float X, Y, Z;
    float L, u, v;
    float ref_U, ref_V;
    float h, C, s;
    // Illuminants for equal energy (CIE 1931)
    float ref_x = 100, ref_y = 100, ref_z = 100; 
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            red = get_pixel(im, i, j, 0);
            green = get_pixel(im, i, j, 1);
            blue = get_pixel(im, i, j, 2);

            // RGB to XYZ
            if (red > 0.04045) {
                var_R = pow(((red + 0.055) / 1.055), 2.4);
            }
            else {
                var_R = red / 12.92;
            }
            if (green > 0.04045) {
                var_G = pow(((green + 0.055) / 1.055), 2.4);
            }
            else {
                var_G = green / 12.92;
            }
            if (blue > 0.04045) {
                var_B = pow(((blue + 0.055) / 1.055), 2.4);
            }
            else {
                var_B = blue / 12.92;
            }

            var_R = var_R * 100;
            var_G = var_G * 100;
            var_B = var_B * 100;

            X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
            Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
            Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

            // XYZ to CIELUV
            var_U = (4 * X) / (X + (15 * Y) + (3 * Z));
            var_V = (9 * Y) / (X + (15 * Y) + (3 * Z));

            var_Y = Y / 100;
            if (var_Y > 0.008856) {
                var_Y = pow(var_Y, 1.0/3);
            }
            else {
                var_Y = (7.787 * var_Y) + (16.0 / 116);
            }

            ref_U = (4 * ref_x) / (ref_x + (15 * ref_y) + (3 * ref_z));
            ref_V = (9 * ref_y) / (ref_x + (15 * ref_y) + (3 * ref_z));

            L = (116 * var_Y) - 16;
            u = 13 * L * (var_U - ref_U);
            v = 13 * L * ( var_V - ref_V);

            // CIELUV to HCL
            h = atan2(v, u);
            C = sqrt(pow(u, 2) + pow(v, 2));
            s = 13 * sqrt(pow(u - ref_x, 2) + pow(v - ref_y, 2));

            set_pixel(im, i, j, 0, h);
            set_pixel(im, i, j, 1, C);
            set_pixel(im, i, j, 2, s);
        }
    }
}

// Source: https://www.easyrgb.com/en/math.php
void hcl_to_rgb(image im) {
    float red, green, blue;
    float var_U, var_V, var_Y;
    float X, Y, Z;
    float L, u, v;
    float ref_U, ref_V;
    float h, C, s;
    // Illuminants for equal energy (CIE 1931)
    float ref_x = 100, ref_y = 100, ref_z = 100; 
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            h = get_pixel(im, i, j, 0);
            C = get_pixel(im, i, j, 1);
            s = get_pixel(im, i, j, 2);

            // HCL to CIELUV
            u = C * cos(h);
            v = C * sin(h);
            L = C / s;

            // CIELUV to XYZ
            var_Y = (L + 16) /116;
            if (pow(var_Y, 3)  > 0.008856) {
                var_Y = pow(var_Y, 3);
            }
            else {
                var_Y = (var_Y - 16 / 116) / 7.787;
            }

            ref_U = (4 * ref_x) / (ref_x + (15 * ref_y) + (3 * ref_z));
            ref_V = (9 * ref_y) / (ref_x + (15 * ref_y) + (3 * ref_z));

            var_U = u / (13 * L) + ref_U;
            var_V = v / (13 * L) + ref_V;

            Y = var_Y * 100;
            X =  -(9 * Y * var_U) / ((var_U - 4) * var_V - var_U * var_V);
            Z = (9 * Y - (15 * var_V * Y) - (var_V * X)) / (3 * var_V);

            // XYZ to RGB
            X = X / 100;
            Y = Y / 100;
            Y = Z / 100;

            red = X *  3.2406 + Y * -1.5372 + Z * -0.4986;
            green = X * -0.9689 + Y *  1.8758 + Z *  0.0415;
            blue = X *  0.0557 + Y * -0.2040 + Z *  1.0570;

            if ( red > 0.0031308 ) {
                red = 1.055 * ( pow(red, 1 / 2.4 ) ) - 0.055;
            }
            else {
                red = 12.92 * red;
            }
            if ( green > 0.0031308 ) {
                green = 1.055 * ( pow(green, 1 / 2.4 ) ) - 0.055;
            }
            else {
                green = 12.92 * green;
            }
            if ( blue > 0.0031308 ) {
                blue = 1.055 * ( pow(blue, 1 / 2.4 ) ) - 0.055;
            }
            else {
                blue = 12.92 * blue;
            }
            
            set_pixel(im, i, j, 0, red);
            set_pixel(im, i, j, 1, green);
            set_pixel(im, i, j, 2, blue);
        }
    }
}
