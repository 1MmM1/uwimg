#include <math.h>
#include <stdlib.h>
#include "image.h"
#include "matrix.h"

// Run an activation function on each element in a matrix,
// modifies the matrix in place
// matrix m: Input to activation function
// ACTIVATION a: function to run
void activate_matrix(matrix m, ACTIVATION a)
{
    int i, j;
    for(i = 0; i < m.rows; ++i){
        double sum = 0;
        for(j = 0; j < m.cols; ++j){
            double x = m.data[i][j];
            if(a == LOGISTIC){
                // TODO
                m.data[i][j] = exp(x) / (1 + exp(x));
            } else if (a == RELU){
                // TODO
                m.data[i][j] = x > 0 ? x : 0;
            } else if (a == LRELU){
                // TODO
                m.data[i][j] = x > 0 ? x : 0.1 * x;
            } else if (a == SOFTMAX){
                // TODO
                m.data[i][j] = exp(x);
            }
            sum += m.data[i][j];
        }
        if (a == SOFTMAX) {
            // TODO: have to normalize by sum if we are using SOFTMAX
            for(j = 0; j < m.cols; ++j){
                m.data[i][j] /= sum;
            }
        }
    }
}

// Calculates the gradient of an activation function and multiplies it into
// the delta for a layer
// matrix m: an activated layer output
// ACTIVATION a: activation function for a layer
// matrix d: delta before activation gradient
void gradient_matrix(matrix m, ACTIVATION a, matrix d)
{
    int i, j;
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            double x = m.data[i][j];
            // TODO: multiply the correct element of d by the gradient
            if(a == LOGISTIC){
                // TODO
                d.data[i][j] *= x * (1 - x);
            } else if (a == RELU){
                // TODO
                d.data[i][j] *= x > 0 ? 1 : 0;
            } else if (a == LRELU){
                // TODO
                d.data[i][j] *= x > 0 ? 1 : 0.1;
            }
        }
    }
}

// Forward propagate information through a layer
// layer *l: pointer to the layer
// matrix in: input to layer
// returns: matrix that is output of the layer
matrix forward_layer(layer *l, matrix in)
{

    l->in = in;  // Save the input for backpropagation


    // TODO: fix this! multiply input by weights and apply activation function.
    matrix out = matrix_mult_matrix(in, l->w);
    activate_matrix(out, l->activation);


    free_matrix(l->out);// free the old output
    l->out = out;       // Save the current output for gradient calculation
    return out;
}

// Backward propagate derivatives through a layer
// layer *l: pointer to the layer
// matrix delta: partial derivative of loss w.r.t. output of layer
// returns: matrix, partial derivative of loss w.r.t. input to layer
matrix backward_layer(layer *l, matrix delta)
{
    // 1.4.1
    // delta is dL/dy
    // TODO: modify it in place to be dL/d(xw)
    gradient_matrix(l->out, l->activation, delta);


    // 1.4.2
    // TODO: then calculate dL/dw and save it in l->dw
    free_matrix(l->dw);
    matrix m_transpose = transpose_matrix(l->in);
    matrix dw = matrix_mult_matrix(m_transpose, delta);
    l->dw = dw;
    free_matrix(m_transpose);

    // 1.4.3
    // TODO: finally, calculate dL/dx and return it.
    matrix w_transpose = transpose_matrix(l->w);
    matrix dx = matrix_mult_matrix(delta, w_transpose);
    free_matrix(w_transpose);
    
    return dx;
}

// Update the weights at layer l
// layer *l: pointer to the layer
// double rate: learning rate
// double momentum: amount of momentum to use
// double decay: value for weight decay
void update_layer(layer *l, double rate, double momentum, double decay)
{
    // TODO:
    // Calculate Δw_t = dL/dw_t - λw_t + mΔw_{t-1}
    // save it to l->v
    matrix temp1 = axpy_matrix(-decay, l->w, l->dw);
    matrix temp2 = axpy_matrix(momentum, l->v, temp1);
    free_matrix(l->v);
    l->v = temp2;

    // Update l->w
    matrix temp3 = axpy_matrix(rate, l->v, l->w);
    free_matrix(l->w);
    l->w = temp3;

    // Remember to free any intermediate results to avoid memory leaks
    free_matrix(temp1);
}

// Make a new layer for our model
// int input: number of inputs to the layer
// int output: number of outputs from the layer
// ACTIVATION activation: the activation function to use
layer make_layer(int input, int output, ACTIVATION activation)
{
    layer l;
    l.in  = make_matrix(1,1);
    l.out = make_matrix(1,1);
    l.w   = random_matrix(input, output, sqrt(2./input));
    l.v   = make_matrix(input, output);
    l.dw  = make_matrix(input, output);
    l.activation = activation;
    return l;
}

// Run a model on input X
// model m: model to run
// matrix X: input to model
// returns: result matrix
matrix forward_model(model m, matrix X)
{
    int i;
    for(i = 0; i < m.n; ++i){
        X = forward_layer(m.layers + i, X);
    }
    return X;
}

// Run a model backward given gradient dL
// model m: model to run
// matrix dL: partial derivative of loss w.r.t. model output dL/dy
void backward_model(model m, matrix dL)
{
    matrix d = copy_matrix(dL);
    int i;
    for(i = m.n-1; i >= 0; --i){
        matrix prev = backward_layer(m.layers + i, d);
        free_matrix(d);
        d = prev;
    }
    free_matrix(d);
}

// Update the model weights
// model m: model to update
// double rate: learning rate
// double momentum: amount of momentum to use
// double decay: value for weight decay
void update_model(model m, double rate, double momentum, double decay)
{
    int i;
    for(i = 0; i < m.n; ++i){
        update_layer(m.layers + i, rate, momentum, decay);
    }
}

// Find the index of the maximum element in an array
// double *a: array
// int n: size of a, |a|
// returns: index of maximum element
int max_index(double *a, int n)
{
    if(n <= 0) return -1;
    int i;
    int max_i = 0;
    double max = a[0];
    for (i = 1; i < n; ++i) {
        if (a[i] > max){
            max = a[i];
            max_i = i;
        }
    }
    return max_i;
}

// Calculate the accuracy of a model on some data d
// model m: model to run
// data d: data to run on
// returns: accuracy, number correct / total
double accuracy_model(model m, data d)
{
    matrix p = forward_model(m, d.X);
    int i;
    int correct = 0;
    for(i = 0; i < d.y.rows; ++i){
        if(max_index(d.y.data[i], d.y.cols) == max_index(p.data[i], p.cols)) ++correct;
    }
    return (double)correct / d.y.rows;
}

// Calculate the cross-entropy loss for a set of predictions
// matrix y: the correct values
// matrix p: the predictions
// returns: average cross-entropy loss over data points, 1/n Σ(-ylog(p))
double cross_entropy_loss(matrix y, matrix p)
{
    int i, j;
    double sum = 0;
    for(i = 0; i < y.rows; ++i){ // y.rows = 128
        for(j = 0; j < y.cols; ++j){
            sum += -y.data[i][j]*log(p.data[i][j]);
        }
    }
    return sum/y.rows;
}


// Train a model on a dataset using SGD
// model m: model to train
// data d: dataset to train on
// int batch: batch size for SGD
// int iters: number of iterations of SGD to run (i.e. how many batches)
// double rate: learning rate
// double momentum: momentum
// double decay: weight decay
void train_model(model m, data d, int batch, int iters, double rate, double momentum, double decay)
{
    int e;
    for(e = 0; e < iters; ++e){
        data b = random_batch(d, batch); // X: [128,785]  y: [128,10]
        matrix p = forward_model(m, b.X);
        fprintf(stderr, "%06d: Loss: %f\n", e, cross_entropy_loss(b.y, p));
        matrix dL = axpy_matrix(-1, p, b.y); // partial derivative of loss dL/dy
        backward_model(m, dL);
        update_model(m, rate/batch, momentum, decay);
        free_matrix(dL);
        free_data(b);
    }
}


// Questions
//
// 5.2.2.1 Why might we be interested in both training accuracy and testing accuracy? What do these two numbers tell us about our current model?
// Training accuracy tells us how well our model fits and can predict the data it was trained on and testing accuracy telss us how well our model fits and 
// can predict the data in our test set, which it was not trained on (how well it may generalize to unseen data). We got a training accuracy of 90.388% 
// and test accuracy of 91.03%. Since our test accuracy is higher than our training, it tells us that our model will likely generalize well to unseen data.
// Both train and test accuracies are high, which means that our model performs rather well on MNIST data.
//
// 5.2.2.2 Try varying the model parameter for learning rate to different powers of 10 (i.e. 10^1, 10^0, 10^-1, 10^-2, 10^-3) and training the model. What 
// patterns do you see and how does the choice of learning rate affect both the loss during training and the final model accuracy?
// For the following trials we used the default weight decay of 0, momentum of 0.9, batch size of 128, and iteration number of 1000.
// 10^1: loss became -nan really quickly (around iteration 3). Training and test accuracy were both around 9.8%
// 10^0: loss fluctuated throughout training between a little over 1 and 0.3. Training and test accuracy were both around 89% (89.34% and 89.33% respectively)
// 10^-1: loss consistently decreased throughout training. Training and test accuracy were both around 92% (91.93% and 91.82% respectively)
// 10^-2: loss consistently decreased throughout training. Training and test accuracy were both around 91% (90.38% and 91.03% respectively)
// 10^-3: loss consistently decreased throughout training. Training accuracy was 85.79% and test accuracy was 86.67%.
// Learning rates greater than one seems to cause the loss to fluctuate greatly during training, and learning rates which were too high (>= 10) caused the model to 
// not learn. Learning rates smaller than 10^-3 seem to learn too slowly so it doesn't reach the same degree of accuracy in the same number of iterations. The 
// 10^-1 and 10^-2 learning rates gave us the highest model accuracy and most stable loss progression during training.
//
// 5.2.2.3 Try varying the parameter for weight decay to different powers of 10: (10^0, 10^-1, 10^-2, 10^-3, 10^-4, 10^-5). How does weight decay affect the 
// final model training and test accuracy?
// For the following trials we used the default learning rate of 0.01, momentum of 0.9, batch size of 128, and iteration number of 1000.
// 10^0: Training accuracy was 89% and test accuracy was 90%
// 10^-1: Training accuracy was 90.27% and test accuracy was slightly higher at 90.88%
// 10^-2: Training accuracy was 90.32% and test accuracy was slightly higher at 90.91%
// 10^-3: Training accuracy was 90.33% and test accuracy was slightly higher at 90.93%
// 10^-4: Training accuracy was 90.33% and test accuracy was slightly higher at 90.93% (same as 10^-3)
// 10^-5: Training accuracy was 90.33% and test accuracy was slightly higher at 90.93% (same as 10^-4)
// It seems like weight decay only affects the model training and test accuracy up to a certain point - in our case, any weight decay smaller than 10^-3
// produced the same training and test accuracy.
//
// 5.2.3.1 Currently the model uses a logistic activation for the first layer. Try using a the different activation functions we programmed. How well do they 
// perform? What's best?
// For the following trials we used the default learning rate of 0.01, weight decay of 0, momentum of 0.9, batch size of 128, and iteration number of 1000.
// Linear: Training accuracy was 91.39% and test accuracy was 91.63%
// Softmax: Training accuracy was 59.42% and test accuracy was 59.82%
// Logistic: Training accuracy was 88.71% and test accuracy was 89.22%
// ReLU: Training accuracy was 92.57% and test accuracy was 92.64%
// Leaky ReLU: Training accuracy was 92.13% and test accuracy was 92.3%
// The best activation function seems to be ReLu since the test accuracy was the highest.
//
// 5.2.3.2 Using the same activation, find the best (power of 10) learning rate for your model. What is the training accuracy and testing accuracy?
// For the following trials we used the default weight decay of 0, momentum of 0.9, batch size of 128, and iteration number of 1000.
// 10^1: Training accuracy was 9.87% and test accuracy was 9.8%
// 10^0: Training accuracy was 19.83% and test accuracy was 19.89%
// 10^-1: Training accuracy was 96.14% and test accuracy was 95.44%
// 10^-2: Training accuracy was 92.57% and test accuracy was 92.64%
// 10^-3: Training accuracy was 86.32% and test accuracy was 86.80%
// We used ReLU for our activation fuction following our results from question 5.2.3.1. From these trials, we found that using a learning rate of 0.1 produced
// the best training and test accuracies than the other learning rates.
//
// 5.2.3.3 Right now the regularization parameter `decay` is set to 0. Try adding some decay to your model. What happens, does it help? Why or why not may this be?
// For the following trials we used the default learning rate of 0.01, momentum of 0.9, batch size of 128, and iteration number of 1000.
// 0: Training accuracy was 88.63% and test accuracy was 89.23%
// 10^0: Training accuracy was 87.24% and test accuracy was 87.85%
// 10^-1: Training accuracy was 88.53% and test accuracy was 89.08%
// 10^-2: Training accuracy was 88.61% and test accuracy was 89.21%
// 10^-3: Training accuracy was 88.63% and test accuracy was 89.23% 
// 10^-4: Training accuracy was 88.63% and test accuracy was 89.23% (same as 10^-3)
// 10^-5: Training accuracy was 88.63% and test accuracy was 89.23% (same as 10^-4)
// We used ReLU for our activation fuction following our results from question 5.2.3.1. Based on our tests, it doesn't seem like adding weight decay improved our 
// accuracy at all; without weight decay our test accuracy was 89.23% and with weight decay our best test accuracy was 89.23%. This might be because ???
//
// 5.2.3.4 Modify your model so it has 3 layers instead of two. The layers should be `inputs -> 64`, `64 -> 32`, and `32 -> outputs`. Also modify your model to train 
// for 3000 iterations instead of 1000. Look at the training and testing error for different values of decay (powers of 10, 10^-4 -> 10^0). Which is best? Why?
// TODO
//
// 5.3.2.1 How well does your network perform on the CIFAR dataset?
// TODO
//
