from uwimg import *
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-box7")

# Perform thumbnail operation
thumb = nn_resize(blur, blur.w//7, blur.h//7)
save_image(thumb, "dogthumb")