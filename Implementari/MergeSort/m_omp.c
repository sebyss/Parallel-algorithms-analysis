
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <stdlib.h>

unsigned int seed = 1;
const unsigned int rand_max = 32768;

double rendom(){
 #pragma omp threadprivate(seed)
 unsigned int rv;
 seed = seed * 1103515245 + 12345;
 rv = ((unsigned)(seed/65536) % rand_max);

 return(double)rv/rand_max;
}


void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    int *L, *R; //temp arrays for left and right

    L = (int*)malloc(n1*sizeof(int));
    R = (int*)malloc(n2*sizeof(int));

    for (i = 0; i < n1; i++) // copy elems
        L[i] = arr[l + i];

    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];

    i = 0; // index of 1st array
    j = 0; // index of 2nd array
    k = l; // index of merged array
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // remaining elements of L[]
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    // remaining elements of R[]
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void serial_merge_sort(int list[], int l, int r){
  if(l <r)
  {
    int m = l + (r-l)/2; //avoid overflow for big l and r
    serial_merge_sort(list, l , m);
    serial_merge_sort(list, m+1, r);
    merge(list, l, m, r);
  }
}

void merge_sort(int list[], int l, int r, int threads){
  if(l <r)
  {
    int m = l + (r-l)/2; //avoid overflow for big l and r
    if(threads > 1)
    {
      #pragma omp parallel
      {
        #pragma omp single
        {
          #pragma omp task
          {
            merge_sort(list, l , m, threads/2);
          }
          #pragma omp task
          {
            merge_sort(list, m+1, r, threads/2);
          }
          #pragma omp taskwait
          #pragma omp task
          {
            merge(list, l, m, r);
          }
        }
      }
    }
    else //does it in serial
    {
      serial_merge_sort(list, l, r);
    }
  }
}

#define N 100000000

int main(){
 
  int threads = 4;

  int n = N; 
  int *list;
  int i = 0;

  list = (int*)malloc(N*sizeof(int));
  
  for(i = 0; i<n;i++)
	list[i] = rendom();

  double start = omp_get_wtime();
  
  omp_set_nested(1);

  int l = 0;
  int r = n-1;
  
  printf("Threads: %d\n",threads);
  merge_sort(list,l,r, threads);
  
  double end = omp_get_wtime(); 
  printf("Time: %f\n", end-start);

  return 0;
}
