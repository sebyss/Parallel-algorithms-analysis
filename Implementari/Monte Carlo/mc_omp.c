//.................................//
// export OMP_NUM_THREADS= 4       //
// gcc -o mc_omp mc_omp.c -fopenmp //
// ./mc_mpi                        //
//.................................//


#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

unsigned int seed = 1;
const unsigned int rand_max = 32768;

double rendom(){
 #pragma omp threadprivate(seed)
 unsigned int rv;
 seed = seed * 1103515245 + 12345;
 rv = ((unsigned)(seed/65536) % rand_max);

 return(double)rv/rand_max;
}

int main(){
  
  long int n = 300000000;
  int i,count;
  double x,y,z, pi;
  double start = omp_get_wtime();
  int core;
  count = 0;

  #pragma omp threadprivate(seed)
  #pragma omp parallel 
  {
     core = omp_get_num_threads();
     seed = omp_get_thread_num()+1;
  }
     #pragma omp parallel for private(x,y,z) reduction(+:count)
  
     for(i = 0; i <= n / core; i++){

        x = rendom();
        y = rendom();
        z = x*x + y*y;

        if (z < 1){
	  count++;	
	}
     }
  
  pi = 4. * count / n * core;
  printf("Value of PI: %f \n",pi); 
  printf("Execution Time: %f \n", omp_get_wtime() - start);
  printf("Number of Cores: %d \n", core);
  return 0;

}






