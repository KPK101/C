#define TILE_SIZE 36
#define RADIUS 2
#include <stdio.h>

__constant__ float kConst[2*RADIUS+1][2*RADIUS+1];

__device__ void printsharedmem(float xshared[TILE_SIZE][TILE_SIZE], int blockx=0, int blocky=0, char t='d'){
    int row = threadIdx.y;
	int col = threadIdx.x;
    int tradius = RADIUS;

    
    if(blockIdx.x==blockx && blockIdx.y==blocky && (t=='d' || t=='0')) printf("printsm :: (%d, %d), val = %f\n",row+tradius,col+tradius,xshared[row+tradius][col+tradius]);

    //TOP
    if(blockIdx.x==blockx && blockIdx.y==blocky && (t=='t' || t=='0')){
        if(row==0){
            for(int i=1; i<=tradius; i++){
                printf("toprow(%d,%d):: (%d, %d), val = %f\n",blockx,blocky,row+tradius-i,col+tradius,xshared[row+tradius-i][col+tradius]);
            }
        }
    }

    //BOTTOM
    if(blockIdx.x==blockx && blockIdx.y==blocky && (t=='b' || t=='0')){
        if(row==blockDim.y-1){
            
            for(int i=1; i<=tradius; i++){
                int ridx = row + tradius + i;
                float value = xshared[ridx][col+tradius];
                printf("bottomrow(%d,%d):: (%d, %d), val = %f\n",blockx,blocky,ridx,col+tradius,value);
            }
        }
    }
    //LEFT
     if(blockIdx.x==blockx && blockIdx.y==blocky && (t=='l' || t=='0')){
        if(col==0){
            for(int j=1; j<=tradius; j++){
                float value = xshared[row+tradius][col+tradius-j];
                printf("leftcol(%d,%d):: (%d, %d), val = %f\n",blockx,blocky,row+tradius,col+tradius-j,value);
            }
        }
    }
    //RIGHT
     if(blockIdx.x==blockx && blockIdx.y==blocky && (t=='r' || t=='0')){
        if(col==blockDim.x-1){
            for(int j=1; j<=tradius; j++){
                float value = xshared[row+tradius][col+tradius+j];
                printf("rightcol(%d,%d):: (%d, %d), val = %f\n",blockx,blocky,row+tradius,col+tradius+j,value);
            }
        }
    }
    //TOP-LEFT CORNER
    if(blockIdx.x==blockx && blockIdx.y==blocky && (t=='1' || t=='0')){
        if(row==0 && col==0){
            for(int i=0; i<tradius; i++){
                for(int j=0; j<tradius; j++){
                    float value = xshared[i][j];
                    printf("topleftcorner(%d,%d):: (%d, %d), val = %f\n",blockx,blocky,i,j,value);
                }
            }
        }
    }

    //TOP-RIGHT CORNER
    if(blockIdx.x==blockx && blockIdx.y==blocky && (t=='2' || t=='0')){
        if(row==0 && col==blockDim.x-1){
            for(int i=0; i<tradius; i++){
                for(int j=0; j<tradius; j++){
                    int colidx = tradius + blockDim.x + j;
                    float value = xshared[i][colidx];
                    printf("toprightcorner(%d,%d):: (%d, %d), val = %f\n",blockx,blocky,i,colidx,value);
                }
            }
        }
    }

    //BOTTOM-LEFT CORNER
    if(blockIdx.x==blockx && blockIdx.y==blocky && (t=='3' || t=='0')){
        if(row==blockDim.y-1 && col==0){
            for(int i=0; i<tradius; i++){
                for(int j=0; j<tradius; j++){
                    int rowidx = tradius + blockDim.y + i;
                    float value = xshared[rowidx][j];
                    printf("bottomleftcorner(%d,%d):: (%d, %d), val = %f\n",blockx,blocky,rowidx,j,value);
                }
            }
        }
    }

    //BOTTOM-RIGHT CORNER
    if(blockIdx.x==blockx && blockIdx.y==blocky && (t=='4' || t=='0')){
        if(row==blockDim.y-1 && col==blockDim.x-1){
            for(int i=0; i<tradius; i++){
                for(int j=0; j<tradius; j++){
                    int rowidx = tradius + blockDim.y + i;
                    int colidx = tradius + blockDim.x + j;
                    float value = xshared[rowidx][colidx];
                    printf("bottomrightcorner(%d,%d):: (%d, %d), val = %f\n",blockx,blocky,rowidx,colidx,value);
                }
            }
        }
    }

    
    

    
}
    


__device__ void copyToSharedMem(float *x, float xshared[TILE_SIZE][TILE_SIZE], int xwidth, int xheight, int kwidth, int kheight){

	// device function to copy data into shared memory
	// break data into TILE_SIZE > block_size because conv requires data more than in input
	int row = threadIdx.y;
	int col = threadIdx.x;

	int global_row = row + blockIdx.y*blockDim.y;
	int global_col = col + blockIdx.x*blockDim.x;

	int tradius = kheight/2;
    

	int col_end = blockDim.x-1;
	int row_end = blockDim.y-1;

    int maxidx = xwidth*xheight - 1;

	// first copy every index to corresponding location in the shared memory
	xshared[row + tradius][col + tradius] = x[global_row*xwidth + global_col];
    // printf("hi test %f",x[0]);

	//All edge cases

	/* How to fill the edges?

	For each thread in first row, assign loading of top "radius" number of elements
	For each thread in last row, assign loading of bottom "radius" number of elements
	For each thread in first col, assign loading of left "radius" number of elements
	For each thread in last col, assign loading of right "radius" number of elements

	For each thread in the corner (0,0) (0, tile_size) (tile_size, 0) (tile_size, tile_size)
	assign loading of the corner block of size (radius, radius)

	*/

    if(row==0){
        
        for(int i=0; i<=tradius; i++){   
            int pos = (global_row-tradius+i)*xwidth + global_col;
            if(pos>=0){
                xshared[i][col+tradius] = x[pos];    
            }
            else{
                xshared[i][col+tradius] = 0;
            }               
        }
    }
    //first load top row
    __syncthreads();
    
    if(row==row_end){

        
        for(int i=0; i<tradius; i++){
            int pos = (global_row + i + 1)*xwidth + global_col;
            if(pos>=0 && pos<=maxidx){    
                xshared[row_end + tradius + i + 1][col + tradius] = x[pos];    

            }
            else{

                xshared[row_end + tradius + i + 1][col + tradius] = 0;
            }
        }   
    
    }
    //second load bottom row
    __syncthreads();

    
	if(col==0){
        
		for(int j=0; j<tradius; j++){
			int pos = global_row*xwidth + global_col - tradius + j;
            if(pos>=0 && pos<=maxidx && global_col!=0){    
                xshared[row + tradius][j] = x[pos];
            }
            else{
                xshared[row + tradius][j] = 0;
            }
        }
	}

    //third load left column
    __syncthreads();

	if(col==col_end){
        
    	for(int j=0; j<tradius; j++){
			int pos = global_row*xwidth + global_col + j + 1;
            if(pos>=0 && pos<=maxidx && global_col!=xwidth-1){    
                xshared[row + tradius][col_end + tradius + j + 1] = x[pos];
            }
            else{
                xshared[row + tradius][col_end + tradius + j + 1] = 0;
            }
        }
    }

    //fourth load right col
    __syncthreads();

    if(row==0&&col==0){
        for(int i=0; i<tradius; i++){
            for(int j=0; j<tradius; j++){
                int pos = (global_row - tradius + i)*xwidth + global_col - tradius + j;
                if(pos>=0 && global_row!=0 && global_col!=0){
                    xshared[i][j] = x[pos];
                }
                else{
                    xshared[i][j] = 0;
                }
            }
        }
    }
    //fifth load topleft corner
    __syncthreads();

    if(row==0&&col==col_end){

        for(int i=0; i<tradius; i++){
            for(int j=0; j<tradius; j++){
                int pos = (global_row - tradius + i)*xwidth + global_col + j + 1;
                if(pos>=0 && global_row!=0 && global_col!=xwidth-1){
                    xshared[i][tradius + blockDim.x + j ] = x[pos];
                }
                else{
                    xshared[i][tradius + blockDim.x + j ] = 0;
                }
            }
        }
        
    }
    //sixth load topright corner
    __syncthreads();

    if(row==row_end&&col==0){
        for(int i=0; i<tradius; i++){
            for(int j=0; j<tradius; j++){
                
                int pos = (global_row + i + 1)*xwidth + global_col - tradius + j;
                if(pos>=0 && blockIdx.x!=0 && blockIdx.y+1<gridDim.x){
                    xshared[tradius+blockDim.y+i][j] = x[pos];
                }
                else{
                    xshared[tradius+blockDim.y+i][j] = 0;
                }
            
            }
        }
    }
    //seventh load bottomleft corner
    __syncthreads();

    if(row==row_end&&col==col_end){

        for(int i=0; i<tradius; i++){
            for(int j=0; j<tradius; j++){
                
                int pos = (global_row + i + 1)*xwidth + global_col + j + 1;
                if(pos>=0 && blockIdx.x+1<gridDim.x && blockIdx.y+1<gridDim.y){
                    xshared[tradius+blockDim.y+i][tradius + blockDim.x + j] = x[pos];
                }
                else{
                    xshared[tradius+blockDim.y+i][tradius + blockDim.x + j] = 0;
                }
            
            }
        }

        
    }
    //eigth load bottomright corner
    __syncthreads();
}


__global__ void conv2d_naive(float *x, float *k, float *y, int xwidth, int xheight, int kwidth, int kheight){

    int row = threadIdx.y + blockIdx.y*blockDim.y;
    int col = threadIdx.x + blockIdx.x*blockDim.x;

    if(row<xheight && col<xwidth){
        // defined global row and column
        
        float result = 0.0f;
        int radius = kwidth/2;
        // loop through possible values of filter
        for(int krow=0; krow<kheight; krow++){
        	for(int kcol=0; kcol<kwidth; kcol++){
        		int xrow = row - radius + krow;
        		int xcol = col - radius + kcol;
                
        		if(xrow>=0 && xrow<xheight && xcol>=0 && xcol<xwidth){
        			result += x[xrow*xwidth+xcol]*k[krow*kwidth+kcol];
        		}
        	}
        }
        
        y[row*xwidth + col] = result;
        
    }
}


__global__ void conv2d_shared(float *x, float *k, float *y, int xwidth, int xheight, int kwidth, int kheight){

    int global_row = threadIdx.y + blockIdx.y*blockDim.y;
    int global_col = threadIdx.x + blockIdx.x*blockDim.x;

    int row = threadIdx.y;
    int col = threadIdx.x;

    int radius = kwidth/2;
    // int smradius = TILE_SIZE/2;

    __shared__ float xshared[TILE_SIZE][TILE_SIZE];
    copyToSharedMem(x, xshared, xwidth, xheight, 2*radius+1, 2*radius+1);

    __syncthreads();
    
    if(row<xheight && col<xwidth){
        // defined global row and column
        
        float result = 0.0f;
        
        // loop through possible values of filter
        for(int krow=0; krow<kheight; krow++){
        	for(int kcol=0; kcol<kwidth; kcol++){
        		int xrow = row + radius - radius + krow;
        		int xcol = col + radius - radius + kcol;
                
        		if(xrow>=0 && xrow<TILE_SIZE && xcol>=0 && xcol<TILE_SIZE){
        			result += xshared[xrow][xcol]*k[krow*kwidth+kcol];
        		}
        	}
        }
        
        y[global_row*xwidth + global_col] = result;
        
    }
}

__global__ void conv2d_constShared(float *x, float *y, int xwidth, int xheight){

    
    
    int global_row = threadIdx.y + blockIdx.y*blockDim.y;
    int global_col = threadIdx.x + blockIdx.x*blockDim.x;

    int row = threadIdx.y;
    int col = threadIdx.x;

    int radius = RADIUS;

    
    __shared__ float xshared[TILE_SIZE][TILE_SIZE];
    copyToSharedMem(x, xshared, xwidth, xheight, 2*radius+1, 2*radius+1);

    __syncthreads();
    
    if(row<xheight && col<xwidth){
        // defined global row and column
        
        float result = 0.0f;
        
        // loop through possible values of filter
        for(int krow=0; krow<2*radius+1; krow++){
        	for(int kcol=0; kcol<2*radius+1; kcol++){
        		int xrow = row + krow ;
        		int xcol = col + kcol ;
                
        		if(xrow>=0 && xrow<TILE_SIZE && xcol>=0 && xcol<TILE_SIZE){
        			result += xshared[xrow][xcol]*kConst[krow][kcol];
        		}
        	}
        }
        
        y[global_row*xwidth + global_col] = result;
        
    }
    
}



__global__ void test_load(float *x, float *k, float *y, int xwidth, int xheight, int kwidth, int kheight){
    // Load a amtrix to shared memory block and print elements to check for correctness
    __shared__ float xshared[TILE_SIZE][TILE_SIZE];
    copyToSharedMem(x, xshared, xwidth, xheight, kwidth, kheight);
    // print loaded shared memory
        // select block to view
        // select mode - (t,b,l,r,1,2,3,4,0)
    printsharedmem(xshared, 0, 0, '0');  


}
