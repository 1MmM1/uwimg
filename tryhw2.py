from uwimg import *
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-box7")

# Perform thumbnail operation
thumb = nn_resize(blur, blur.w//7, blur.h//7)
save_image(thumb, "dogthumb")

# Use highpass filter
f = make_highpass_filter()
highpass = convolve_image(im, f, 0)
save_image(highpass, "dog-highpass")

# Use sharpen filter
f = make_sharpen_filter()
sharpen = convolve_image(im, f, 1)
save_image(sharpen, "dog-sharpen")

# Use emboss filter
f = make_emboss_filter()
emboss = convolve_image(im, f, 1)
save_image(emboss, "dog-emboss")

# Use Gaussian filter
f = make_gaussian_filter(2)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-gauss2")
