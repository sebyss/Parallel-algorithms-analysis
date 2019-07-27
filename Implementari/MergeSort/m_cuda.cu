#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <time.h>

#define N1000 100000000


__device__ void CudaMerge(int* values, int* results, int l, int r, int u)
{
	int i, j, k;
	i = l; j = r; k = l;

	while (i < r && j < u) {
		if (values[i] <= values[j]) { results[k] = values[i]; i++; }
		else { results[k] = values[j]; j++; }
		k++;
	}

	while (i < r) {
		results[k] = values[i]; i++; k++;
	}

	while (j < u) {
		results[k] = values[j]; j++; k++;
	}
	for (k = l; k < u; k++) {
		values[k] = results[k];
	}
}


__global__ static void CudaMergeSort(int * values, int* results, int dim){
	extern __shared__ int shared[];


	const unsigned int tid = threadIdx.x;
	int k, u, i;
	shared[tid] = values[tid];
	
	__syncthreads();
	k = 1;
	while (k <= dim)
	{
		i = 0;
		while (i + k < dim)
		{
			u = i + k * 2;;
			if (u > dim)
			{
				u = dim + 1;
			}
			CudaMerge(shared, results, i, i + k, u);
			i = i + k * 2;
		}
		k = k * 2;

		__syncthreads();
	}

	values[tid] = shared[tid];
}


void mergeSort(int arr[], int p, int q);


int main(int argc, char** argv)
{	
	float elapsed1000 = 0;

	cudaEvent_t start1000, stop1000;
	
	
	cudaEventCreate(&start1000);
	cudaEventCreate(&stop1000);
	
	int  *values4;
	int  *values4s;
	 
	values4 = (int*)malloc(N1000*sizeof(int));
	values4s = (int*)malloc(N1000*sizeof(int));
	
	int* dvalues, *results;
	
	cudaMalloc((void**)&dvalues, sizeof(int) * N1000);
	cudaMalloc((void**)&results, sizeof(int) * N1000);

	printf("\nElements for N = 1000:\n");
	for (int i = 0; i < N1000; i++)
	{
		values4[i] = rand();
		values4s[i] = values4[i];
		//printf("%d ", values4[i]);
	}

	printf("\n");
	//Start monitorizing of cuda operations
	cudaEventRecord(start1000, 0);
	//Generation cuda variables ables to work and copying the variables from host to device
	
	cudaMemcpy(dvalues, values4, sizeof(int) * N1000, cudaMemcpyHostToDevice);
	
	cudaMemcpy(results, values4s, sizeof(int)* N1000, cudaMemcpyHostToDevice);
	//Calling algorithm MergeSort
	
	CudaMergeSort << <1, N1000, sizeof(int) * N1000 * 2 >> > (dvalues, results, N1000);

	// Freeing memory space used and returning values sortered
	cudaFree(dvalues);
	cudaMemcpy(values4, results, sizeof(int)*N1000, cudaMemcpyDeviceToHost);
	cudaFree(results);
	//Stopping time monitoring
	cudaEventRecord(stop1000, 0);
	cudaEventSynchronize(stop1000);
	//Calculating the total time of execution
	cudaEventElapsedTime(&elapsed1000, start1000, stop1000);
	// Freeing the events created before
	cudaEventDestroy(start1000);
	cudaEventDestroy(stop1000);
	//Showing sorted elements
	
	//Showing the time of execution
	printf("\n\t||| The elapsed time in gpu was %.2f ms |||", elapsed1000);
	printf("\n");

	cudaDeviceReset();
	cudaThreadExit();

	

	return 0;
}


void merge(int arr[], int p, int q, int r) {

	int i, j, k;
	int n1 = q - p + 1;
	int n2 = r - q;
	
	//arrs temporales
	int *L, *M;
	cudaMalloc((void**)&L, sizeof(int) * n1);
	cudaMalloc((void**)&M, sizeof(int) * n2);
	cudaMalloc((void**)&arr, sizeof(int) * N1000);
	

	for (int i = 0; i < n1; i++)
	{
		L[i] = arr[p + i];
	}

	for (int j = 0; j < n2; j++)
	{
		M[j] = arr[q + 1 + j];
	}

	i = 0;
	j = 0;
	k = p;

	while (i < n1 && j < n2)
	{
		if (L[i] <= M[j])
		{
			arr[k] = L[i];
			i++;
		}
		else
		{
			arr[k] = M[j];
			j++;
		}
		k++;
	}

	while (i < n1)
	{
		arr[k] = L[i];
		i++;
		k++;
	}

	while (j < n2)
	{
		arr[k] = M[j];
		j++;
		k++;
	}
}

void mergeSort(int arr[], int p, int q) {


	if (p < q)
	{
		int mitad = (p + q) / 2;

		mergeSort(arr, p, mitad);
		mergeSort(arr, mitad + 1, q);
		merge(arr, p, mitad, q);
	}
}
