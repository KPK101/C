# Convolution in CUDA

## Methods
The kernels file includes three implementations of the convolution operation

1) Naive:
    - This is the most simple implemetation of the convolution operation on a GPU. It involves loading input matrices X and K into the global memory and computing the output indices with global memory access. It is very inefficient because of uncoalesced memory access, global memory bandwidth overhead - limitation (can use faster memories to speedup computations) 
2) Shared Memory Optimized
    - The memory operations are optimized by loading a part of the input matrix into the shared memory of each block. This implementation assumes that the output grid and input grid sizes are the same. Therefore one needs to ensure that the shared memory tile is loaded by the required "padding" to enable successful computation; elements on the edge of each block will need adjacent input elements that are out of boundaries the grid. So one needs to add these separately to the shared memory. Because each thread performs part of the loading, we assign threads on the edges the addition task of loading the padding elements. This makes the control divergence worse, but is a necessary operation. Once memories are loaded, thread are synced and subsequent convoltuion per block is performed with a lower memory access latency from the shared memory. However we access the filter from the global memory. Therefore the operation has more scope to be optimized. 
3) Constant Memory Optimized
    - The final optimization involves loading the filter into the constant memory. There are two reasons - the filter is a constant variable that is usually very small and is common to a majority of the computations performed during the convolution operation. In addition to this, when one loads a variable to constant memory, its caching is optimized further which leads to much lower access latencies than when it is stored in the global pool (It is imoprtant to note that constant variables are still stores in the DRAM - global memory, but have better access latencies).
  
## Testing

The kernels are tested using python - pycuda.

This is a convenient method for executing kernels as the host scripting becomes much simpler due to easy definition of data and kernel calling.


## ERRATA
It was observed during testing that the code only works for even indices -powers of 2. The most likely cause is the way in which boundary conditions are treated. This needs to be worked on, and the repo will be updated once it is working!.

## Addition Steps
In addition to generalizing the kernel for non-powers of 2, I intend to add analysis of the kernel performance and how optimizations improved resource usage and execution time. NVIDIA Nsight Compute reports will be used for the same.

In addition, a summary document will be uploaded that will summarize my study of the performance of convolution algorithm on GPUs.