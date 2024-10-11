# C codes

This repository contains C/C++ codes for various programs.

My goal is to develop a good programming practice with these languages by solving tasks that have applications in AI/ML. 
I am developing proficiency in CUDA/OpenCL programming so this experience will help.

The first commit contains a simple program that implements 1D and 2D Convolution (valid and same) and test them.

## Convolution

Convolution is an algorithm primarily used in signal processing.

It has gain popularity in the AI community because of its applications in "Convolutional Neural Networks". This is, however, is a misnomer as the operation performed by CNNs is actually correlation and not strictly convolution. The distinction between the two lies in the fact that convolution essentially "flips" the kernel before computing the overlap sum/integral of the input signal and the filter being used.

Convolution between the input signal $f$ and filter $g$ is defined as:

```math

(f*g)(t) = \sum[\tau]g[t-\tau]

```


