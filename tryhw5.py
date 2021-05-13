import argparse
from uwimg import *

def parse_arguments():
    parser = argparse.ArgumentParser(description = "Homework 5")
    parser.add_argument("--model", help = "Model to use for training: either linear softmax model (softmax), 2 layer neural network (nn2), or 3 layer neural network (nn3)")
    parser.add_argument("--data", help = "Dataset to use: either MNIST (mnist) or CIFAR-10 (cifar)")
    args = parser.parse_args()

    model = args.model if args.model else "softmax"
    dataset = args.data if args.data else "mnist"

    print("Using model:", model)
    print("Using dataset:", dataset)
    print
    return model, dataset

def softmax_model(inputs, outputs):
    l = [make_layer(inputs, outputs, SOFTMAX)]
    return make_model(l)

def neural_net(inputs, outputs):
    print(inputs)
    l = [   make_layer(inputs, 32, LOGISTIC),
            make_layer(32, outputs, SOFTMAX)]
    return make_model(l)

def neural_net_3layer(inputs, outputs):
    l = [   make_layer(inputs, 64, RELU),
            make_layer(64, 32, RELU),
            make_layer(32, outputs, SOFTMAX)]
    return make_model(l)

model, dataset = parse_arguments()

if dataset == "mnist":
    print("loading MNIST data...")
    train = load_classification_data(b"mnist.train", b"mnist.labels", 1)
    test  = load_classification_data(b"mnist.test",  b"mnist.labels", 1)
elif dataset == "cifar":
    print("loading CIFAR-10 data...")
    train = load_classification_data(b"cifar.train", b"cifar/labels.txt", 1)
    test  = load_classification_data(b"cifar.test",  b"cifar/labels.txt", 1)
else:
    print("Dataset not supported")
    exit()
print("done")
print

print("training", model, "model...")
batch = 128
iters = 1000
rate = .01
momentum = .9
decay = .0

if model == "softmax":
    m = softmax_model(train.X.cols, train.y.cols)
elif model == "nn2":
    m = neural_net(train.X.cols, train.y.cols)
elif model == "nn3":
    m = neural_net_3layer(train.X.cols, train.y.cols)
else:
    print("Model not supported")
    exit()
train_model(m, train, batch, iters, rate, momentum, decay)
print("done")
print

print("Model parameters:")
print("batch size:", batch)
print("epochs:", iters)
print("learning rate:", rate)
print("momentum:", momentum)
print("weight decay lambda:", decay)

print("evaluating model...")
print("training accuracy: %f", accuracy_model(m, train))
print("test accuracy:     %f", accuracy_model(m, test))
