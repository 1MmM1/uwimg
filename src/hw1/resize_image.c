#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    // TODO Fill in
    return get_pixel(im, round(x), round(y), c);
}

image nn_resize(image im, int w, int h)
{
    // TODO Fill in (also fix that first line)
    image resized = make_image(w, h, im.c);
    float x_scale = 1.0 * im.w / resized.w;
    float y_scale = 1.0 * im.h / resized.h;
    float x_start = -0.5 + x_scale / 2;
    float y_start = -0.5 + y_scale / 2;
    for (int i = 0; i < resized.w; i++) {
        for (int j = 0; j < resized.h; j++) {
            for (int k = 0; k < resized.c; k++) {
                set_pixel(resized, i, j, k, nn_interpolate(im, x_start + i * x_scale, y_start + j * y_scale, k));
            }
        }
    }
    return resized;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // TODO
    float top = floor(y);
    float bottom = ceil(y);
    float left = floor(x);
    float right = ceil(x);
    float q1 = (bottom - y) * get_pixel(im, left, top, c) + (y - top) * get_pixel(im, left, bottom, c);
    float q2 = (bottom - y) * get_pixel(im, right, top, c) + (y - top) * get_pixel(im, right, bottom, c);
    return (right - x) * q1 + (x - left) * q2;
}

image bilinear_resize(image im, int w, int h)
{
    // TODO
    return make_image(1,1,1);
}
