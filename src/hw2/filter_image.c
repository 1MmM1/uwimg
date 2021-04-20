#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"

void l1_normalize(image im)
{
    float sum = 0;

    // find sum
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            for (int k = 0; k < im.c; k++) {
                sum += get_pixel(im, i, j, k);
            }
        }
    }

    // normalize
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
    image im = make_image(w, w, 1);

    // fill image with ones
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
    assert(filter.c == 1 || filter.c == im.c);

    image result = make_image(1,1,1);
    if (filter.c == im.c && preserve != 1) {
        // apply filter 1:1 to image and sum across all channels; produce 1 channel image
        result = make_image(im.w, im.h, 1);
        for (int i = 0; i < im.w; i++) {
            for (int j = 0; j < im.h; j++) {
                float sum = 0;
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
        // apply filter 1:1 to image; produce im.c channel image
        result = make_image(im.w, im.h, im.c);
        for (int i = 0; i < im.w; i++) {
            for (int j = 0; j < im.h; j++) {
                for (int k = 0; k < im.c; k++) {
                    float sum = 0;
                    for (int q = 0; q < filter.w; q++) {
                        for (int r = 0; r < filter.h; r++) {
                            sum += get_pixel(filter, q, r, k) * get_pixel(im, i - filter.w / 2 + q, j - filter.h / 2 + r, k);
                        }
                    }
                    set_pixel(result, i, j, k, sum);
                }
            }
        }
    } else if (filter.c == 1 && preserve != 1) {
        // apply filter to each channel individually and sum across channels; produce 1 channel image
        result = make_image(im.w, im.h, 1);
        for (int i = 0; i < im.w; i++) {
            for (int j = 0; j < im.h; j++) {
                float sum = 0;
                for (int k = 0; k < im.c; k++) {
                    for (int q = 0; q < filter.w; q++) {
                        for (int r = 0; r < filter.h; r++) {
                            sum += get_pixel(filter, q, r, 0) * get_pixel(im, i - filter.w / 2 + q, j - filter.h / 2 + r, k);
                        }
                    }
                }
                set_pixel(result, i, j, 0, sum);
            }
        }
    } else if (filter.c == 1 && preserve == 1) {
        // apply filter to each channel individually; produce im.c channel image
        result = make_image(im.w, im.h, im.c);
        for (int i = 0; i < im.w; i++) {
            for (int j = 0; j < im.h; j++) {
                for (int k = 0; k < im.c; k++) {
                    float sum = 0;
                    for (int q = 0; q < filter.w; q++) {
                        for (int r = 0; r < filter.h; r++) {
                            sum += get_pixel(filter, q, r, 0) * get_pixel(im, i - filter.w / 2 + q, j - filter.h / 2 + r, k);
                        }
                    }
                    set_pixel(result, i, j, k, sum);
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
    image filter = make_image(3,3,1);
    for (int i = 0; i < filter.w * filter.h; i++) {
        filter.data[i] = i % 2 == 0 ? 0 : -1;
    }
    set_pixel(filter, 1, 1, 0, 4);
    return filter;
}

image make_sharpen_filter()
{
    image filter = make_image(3,3,1);
    for (int i = 0; i < filter.w * filter.h; i++) {
        filter.data[i] = i % 2 == 0 ? 0 : -1;
    }
    set_pixel(filter, 1, 1, 0, 5);
    return filter;
}

image make_emboss_filter()
{
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, -2);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 1, 1, 0, 1);
    set_pixel(filter, 1, 2, 0, 1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 2, 1, 0, 1);
    set_pixel(filter, 2, 2, 0, 2);
    return filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO
// highpass: no preserve. want to find edges.
// sharpen, emboss: preserve. want an rgb image.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO. overflow/underflow

image make_gaussian_filter(float sigma)
{
    int size = ceil(sigma * 6);
    size = size % 2 == 0 ? size + 1 : size;
    image filter = make_image(size, size, 1);
    for (int i = 0; i < filter.w; i++) {
        for (int j = 0; j < filter.h; j++) {
            float x = size / 2 - i; // x-distance from pixel (i,j,0) to the center of the filter
            float y = size / 2 - j; // y-distance from pixel (i,j,0) to the center of the filter
            float G = exp(-0.5 * (x * x + y * y) / (sigma * sigma)) / (TWOPI * sigma * sigma);
            set_pixel(filter, i, j, 0, G);
        }
    }
    l1_normalize(filter);
    return filter;
}

image add_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    image sum_image = make_image(a.w, a.h, a.c);

    for (int i = 0; i < a.w; i++) {
        for (int j = 0; j < a.h; j++) {
            for (int k = 0; k < a.c; k++) {
                set_pixel(sum_image, i, j, k, get_pixel(a, i, j, k) + get_pixel(b, i, j, k));
            }
        }
    }

    return sum_image;
}

image sub_image(image a, image b)
{
    // Subtract b from a

    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    image sub_image = make_image(a.w, a.h, a.c);

    for (int i = 0; i < a.w; i++) {
        for (int j = 0; j < a.h; j++) {
            for (int k = 0; k < a.c; k++) {
                set_pixel(sub_image, i, j, k, get_pixel(a, i, j, k) - get_pixel(b, i, j, k));
            }
        }
    }

    return sub_image;
}

image make_gx_filter()
{
    // TODO
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, -1);
    set_pixel(filter, 0, 1, 0, 0);
    set_pixel(filter, 0, 2, 0, 1);
    set_pixel(filter, 1, 0, 0, -2);
    set_pixel(filter, 1, 1, 0, 0);
    set_pixel(filter, 1, 2, 0, 2);
    set_pixel(filter, 2, 0, 0, -1);
    set_pixel(filter, 2, 1, 0, 0);
    set_pixel(filter, 2, 2, 0, 1);
    return filter;
}

image make_gy_filter()
{
    // TODO
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, -1);
    set_pixel(filter, 0, 1, 0, -2);
    set_pixel(filter, 0, 2, 0, -1);
    set_pixel(filter, 1, 0, 0, 0);
    set_pixel(filter, 1, 1, 0, 0);
    set_pixel(filter, 1, 2, 0, 0);
    set_pixel(filter, 2, 0, 0, 1);
    set_pixel(filter, 2, 1, 0, 2);
    set_pixel(filter, 2, 2, 0, 1);
    return filter;
}

void feature_normalize(image im)
{
    // TODO
    // Find minimum and range
    if (im.w == 0 && im.h == 0 && im.w == 0) return;

    float min = get_pixel(im, 0, 0, 0);
    float max = get_pixel(im, 0, 0, 0);
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            for (int k = 0; k < im.c; k++) {
                float pixel = get_pixel(im, i, j, k);
                if (pixel < min) {
                    min = pixel;
                }
                if (pixel > max) {
                    max = pixel;
                }
            }
        }
    }

    float range = max - min;
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            for (int k = 0; k < im.c; k++) {
                if (range == 0) {
                    set_pixel(im, i, j, k, 0);
                } else {
                    set_pixel(im, i, j, k, (get_pixel(im, i, j, k) - min) / range);
                }
            }
        }
    }
}

image *sobel_image(image im)
{
    // TODO
    image* res = calloc(2, sizeof(image));
    res[0].w = im.w;
    res[0].h = im.h;
    res[0].c = im.c;
    res[0].data = calloc(im.w * im.h * im.c, sizeof(float));
    res[1].w = im.w;
    res[1].h = im.h;
    res[1].c = im.c;
    res[1].data = calloc(im.w * im.h * im.c, sizeof(float));


    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();

    image gx_convolution = convolve_image(im, gx_filter, 1);
    image gy_convolution = convolve_image(im, gy_filter, 1);

    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            for (int k = 0; k < im.c; k++) {
                float gx = get_pixel(gx_convolution, i, j, k);
                float gy = get_pixel(gy_convolution, i, j, k);
                float magnitude = sqrt(gx * gx + gy * gy);
                float direction = atan(gy / gx);
                set_pixel(res[0], i, j, k, magnitude);
                set_pixel(res[1], i, j, k, direction);
            }
        }
    }

    return res;
}

image colorize_sobel(image im)
{
    // TODO
    return make_image(1,1,1);
}
