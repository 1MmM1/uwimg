#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    // TODO
    float sum = 0;
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            for (int k = 0; k < im.c; k++) {
                sum += get_pixel(im, i, j, k);
            }
        }
    }
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            for (int k = 0; k < im.c; k++) {
                set_pixel(im, i, j, k, get_pixel(im, i, j, k) / sum);
            }
        }
    }
}

image make_box_filter(int w)
{
    // TODO
    image im = make_image(w, w, 1);
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            set_pixel(im, i, j, 0, 1);
        }
    }
    l1_normalize(im);
    return im;
}

image convolve_image(image im, image filter, int preserve)
{
    // TODO
    assert(filter.c == 1 || filter.c == im.c);
    image result = make_image(1,1,1);
    if (filter.c == im.c && preserve != 1) {
        result = make_image(im.w, im.h, 1);
        for (int i = 0; i < im.w; i++) {
            for (int j = 0; j < im.h; j++) {
                // apply filter 1:1 to image and sum all channels
                int sum = 0;
                for (int q = 0; q < filter.w; q++) {
                    for (int r = 0; r < filter.h; r++) {
                        for (int s = 0; s < filter.c; s++) {
                            sum += get_pixel(filter, q, r, s) * get_pixel(im, i - filter.w / 2 + q, j - filter.h / 2 + r, s);
                        }
                    }
                }
                set_pixel(result, i, j, 0, sum);
            }
        }
    } else if (filter.c == im.c && preserve == 1) {
        result = make_image(im.w, im.h, im.c);
        for (int i = 0; i < im.w; i++) {
            for (int j = 0; j < im.h; j++) {
                for (int k = 0; k < im.c; k++) {
                    // apply filter 1:1 to image and each channel individually
                    int sum = 0;
                    for (int q = 0; q < filter.w; q++) {
                        for (int r = 0; r < filter.h; r++) {
                            sum += get_pixel(filter, q, r, k) * get_pixel(im, i - filter.w / 2 + q, j - filter.h / 2 + r, k);
                        }
                    }
                    set_pixel(result, i, j, k, sum);
                }
            }
        }
    } else if (filter.c == 1) {
        for (int i = 0; i < im.w; i++) {
            for (int j = 0; j < im.h; j++) {
                for (int k = 0; k < im.c; k++) {
                    
                }
            }
        }
    } else {
        // Should never get here
        printf("Something went wrong applying convolutions.");
    }
    return result;
}

image make_highpass_filter()
{
    // TODO
    return make_image(1,1,1);
}

image make_sharpen_filter()
{
    // TODO
    return make_image(1,1,1);
}

image make_emboss_filter()
{
    // TODO
    return make_image(1,1,1);
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
    // TODO
    return make_image(1,1,1);
}

image add_image(image a, image b)
{
    // TODO
    return make_image(1,1,1);
}

image sub_image(image a, image b)
{
    // TODO
    return make_image(1,1,1);
}

image make_gx_filter()
{
    // TODO
    return make_image(1,1,1);
}

image make_gy_filter()
{
    // TODO
    return make_image(1,1,1);
}

void feature_normalize(image im)
{
    // TODO
}

image *sobel_image(image im)
{
    // TODO
    return calloc(2, sizeof(image));
}

image colorize_sobel(image im)
{
    // TODO
    return make_image(1,1,1);
}
