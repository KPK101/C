#include<stdio.h>
#include<stdlib.h>


float** initMatrix(int xlen, int ylen){
	// Initialize a matrix of shape (xlen, ylen)
	float** x;
	x = malloc(xlen*sizeof(float*));
	for(int i=0; i<xlen; i++){
		x[i] = malloc(ylen*sizeof(float));
		for(int j=0; j<ylen; j++){
			x[i][j] = 0;
		}
	}
	return x;
}



void printMatrix(float** x, int xlen, int ylen){
	//print the input matrix of shape (xlen, ylen)
	
	for(int i=0; i<xlen; i++){
		printf("[ ");
		for(int j=0; j<ylen; j++){
			printf("%d ",(int)x[i][j]);
		}
		printf("]\n");
	}
	printf("\n");
}


void print_vector(float *x, int size){
	// Print an input vector of length "size"
	printf("[ ");	
	for(int i=0; i<size; i++){
		printf("%d ",(int)x[i]);
	}
	printf("]\n");
}


void print_flip(float *x, int size){
	// Print the flipped version of a vector - helps with visualizing flipped kernels
	printf("[ ");	
	for(int i=0; i<size; i++){
		printf("%d ",(int)x[size-i-1]);
	}
	printf("]\n");
}


void conv1D(float *f, float* g, float* y, int input_size, int filter_size){

	// This function assumes that the kernel size is less than the input size
	// Does it matter - yes

	for(int i=0; i<input_size-filter_size+1; i++){

		y[i] = 0; // Initialize element with zero

		// Loop through all output indices to compute them
		for (int j=0; j<filter_size; j++){
			// Inner loop to compute the summation of overlapping input and filter
			y[i] += f[j+i]*g[filter_size-1-j];
		}
	}

}


float ** conv2D(float** x, float **k, 
	int xlenx, int xleny, int klenx, int kleny){

	int ylenx = xlenx - klenx + 1;
	int yleny = xleny - kleny + 1;
	// Create outmatrix y
	// printf("conv2D computation - output shape = (%d, %d)\n\n", ylenx, ylenx);
	float **y = initMatrix(ylenx, yleny);
	if(y==NULL){
		printf("Unable to allocate memory :(");
		return NULL;
	}

	for(int i=0; i<ylenx; i++){
		for(int j=0; j<yleny; j++){

			// Initialize to zero
			y[i][j]=0;
			// Loop through all overlapping input/kernel indeces
			// printf("Processing (i,j) = (%d,%d)\n",i,j);

			for(int m=0; m<klenx; m++){
				for(int n=0; n<kleny; n++){

					// printf("\t+ prod of %d and %d\n",(int)x[m+i][n+j],(int)k[klenx-1-m][kleny-1-n]);
					
					y[i][j] += x[m+i][n+j]*k[klenx-1-m][kleny-1-n];
					
					// printf("\t\tsum = %d\n",(int)y[i][j]);
				}

		}
		// printf("exiting (%d, %d) now...\n",i,j);
	}

}

return y;

}

float ** conv2D_same(float **x, float **k, int xlenx, int xleny, int klenx, int kleny){

	// Convoltion kernel for same size
	// Input padded
	// Input is half to left and half to right
	

	int leftpadx = (int)(klenx-1)/2;
	int rightpadx = klenx-1-leftpadx;
	int leftpady = (int)(kleny-1)/2;
	int rightpady = kleny-1-leftpady;

	// printf("padded (%d,%d) on left and (%d,%d) on right\n",leftpadx,
	// 	leftpadx, rightpadx, rightpady);
	float ** xpad = initMatrix(xlenx + klenx -1, xleny + kleny -1);
	// printf("Copying data from x to xpad\n");
	for(int i=leftpadx; i<xlenx-(leftpadx); i++){
		for(int j=leftpady; j<xleny-(leftpady); j++){
			// printf("\tProcessing (%d,%d)\n",i,j);	
			xpad[i][j] = x[i-leftpadx][j-leftpady];
		}
	}
	// printf("Copied successfully!\n");

	// printf("conv2D same computation - output shape = (%d, %d)\n\n", xlenx, xleny);
	float ** y = conv2D(xpad, k, xlenx+klenx-1, xleny+kleny-1, klenx, kleny);
	return y;
}	

void test_conv1d(void){

	// Initialize an input vector - f, a kernel/filter - g, an output - y
	// input size
	// filter size
	// each vector varies from 0/1 to vector_size(-0/-1)
	int insize = 5;
	int kernelsize = 2;
	float f[insize];
	float g[kernelsize];

	int outsize = insize - kernelsize + 1;
	float y[outsize];

	// Initialize defiend vectors with values
	for(int i=0; i<insize; i++){
		f[i] = i;
	}
	for(int i=0; i<kernelsize; i++){
		g[i] = i+1;
	}

	// call the conv1d function - this performs valid convolution on the input array
	conv1D(f, g, y, insize, kernelsize);

	printf("\nTesting 1D convolution...\n");
	
	// printf(" f size = %d\ng size = %d\n", insize, kernelsize);
	
	printf("f of size (%d):\n\n",insize);
	print_vector(f,insize);
	
	printf("\n\"flipped\" g of size (%d):\n\n", kernelsize);
	print_flip(g, kernelsize);

	// printf("The convolution operation is finished:\n");
	printf("\ny of size (%d):\n\n", outsize);
	print_vector(y, outsize);
}

void test_conv2d(void){

	int xlenx = 3;
	int xleny = 3;
	int klenx = 2;
	int kleny = 2;

	
	float **x = initMatrix(xlenx, xleny);
	float **k = initMatrix(klenx, kleny);


	for(int i=0;i<xlenx;i++){
		for(int j=0;j<xleny;j++){
			x[i][j] = (i)*xlenx + j;
		}
	}


	for(int i=0;i<klenx;i++){
		for(int j=0;j<kleny;j++){
			k[i][j] = i;
		}
	}

	printf("Testing 2D convolution...\n");

	printf("x of shape (%d, %d) =\n\n", xlenx, xleny);
	printMatrix(x, xlenx, xleny);

	printf("k of shape (%d, %d) =\n\n", klenx, kleny);
	printMatrix(k, klenx, kleny);



	float **y = conv2D(x, k, xlenx, xleny, klenx, kleny);
	printf("y of shape (%d, %d) =\n\n", xlenx - klenx + 1, xleny - kleny + 1);
	printMatrix(y, xlenx - klenx + 1, xleny - kleny + 1);


	float **y_same = conv2D_same(x, k, xlenx, xleny, klenx, kleny);
	printf("y_same of shape (%d, %d) =\n\n", xlenx , xleny);
	printMatrix(y_same, xlenx, xleny);


}

int main(void){

test_conv1d();
printf("\n");
test_conv2d();

return 0;
}