from uwimg import *
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-box7")

# Perform thumbnail operation
thumb = nn_resize(blur, blur.w//7, blur.h//7)
save_image(thumb, "dogthumb")

def optimize_for_display(image):
    for i in range(image.w):
        for j in range(image.h):
            for k in range(image.c):
               val = min(max(0, get_pixel(image, i, j, k)), 1)
               set_pixel(image, i, j, k, val)

# Use highpass filter
f = make_highpass_filter()
highpass = convolve_image(im, f, 0)
optimize_for_display(highpass)
save_image(highpass, "dog-highpass")

# Use sharpen filter
f = make_sharpen_filter()
sharpen = convolve_image(im, f, 1)
optimize_for_display(sharpen)
save_image(sharpen, "dog-sharpen")

# Use emboss filter
f = make_emboss_filter()
emboss = convolve_image(im, f, 1)
optimize_for_display(emboss)
save_image(emboss, "dog-emboss")

# Use Gaussian filter
f = make_gaussian_filter(2)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-gauss2")

# Dog reconstruction
f = make_gaussian_filter(2)
lfreq = convolve_image(im, f, 1)
hfreq = im - lfreq
reconstruct = lfreq + hfreq
save_image(lfreq, "low-frequency")
save_image(hfreq, "high-frequency")
save_image(reconstruct, "reconstruct")

# Ronbledore

# Sobel image
res = sobel_image(im)
mag = res[0]
feature_normalize(mag)
save_image(mag, "magnitude")

direction = res[1]
feature_normalize(direction)
save_image(direction, "direction")

# Colorized sobel
color_sobel = colorize_sobel(im)
optimize_for_display(color_sobel)
save_image(color_sobel, "dog-colorize-sobel")
