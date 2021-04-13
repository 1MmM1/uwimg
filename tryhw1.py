from uwimg import *
im = load_image("data/dogsmall.jpg")
a = nn_resize(im, im.w*4, im.h*4)
save_image(a, "dog4x-nn")

# Bilinear resize up
im = load_image("data/dogsmall.jpg")
a = bilinear_resize(im, im.w*4, im.h*4)
save_image(a, "dog4x-bl")

# NN resize down
im = load_image("data/dog.jpg")
a = nn_resize(im, im.w//7, im.h//7)
save_image(a, "dog7th-nn")

# Bilinear resize down
im = load_image("data/dog.jpg")
a = bilinear_resize(im, im.w//7, im.h//7)
save_image(a, "dog7th-bl")