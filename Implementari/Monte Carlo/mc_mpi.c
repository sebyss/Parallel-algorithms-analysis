//...........................//
// mpicc -o mc_mpi mc_mpi.c  //
// mpiexec -np 4 ./mc_mpi    //
//...........................//

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>

unsigned int seed = 1;
const unsigned int rand_max = 32768;

double rendom(){
 unsigned int rv;
 seed = seed * 1103515245 + 12345;
 rv = ((unsigned)(seed/65536) % rand_max);

 return(double)rv/rand_max;
}

  

int main(int argc, char * argv[]){
  int myid;
  long int n = 300000000;
  int reduced;
  int numnodes;
  int i = 0;
  double x = 0 , y = 0;
  int count = 0;
  double value_pi = 0;

  MPI_Init (& argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &numnodes);
  
  int counts[numnodes];
  MPI_Request requests[numnodes-1];
  MPI_Status statuses[numnodes-1];
  int temp;
  
  seed = myid+1;
  int t1 = MPI_Wtime();
  
     for (i = 0; i < n / numnodes; i++){
        x = rendom();
        y = rendom();
        if((x * x + y * y) <= 1) count ++;
     }

  
  if(myid != 0){
    MPI_Send(&count,1,MPI_INT,0,1,MPI_COMM_WORLD);
  }
  else{
    counts[0] = count;
    
    for(i = 1; i < numnodes; i++){
       MPI_Irecv(&counts[i],1,MPI_INT,i,1,MPI_COMM_WORLD,&requests[i-1]);
    }
    MPI_Waitall(numnodes-1,requests,statuses);
  }
    
  
  if(myid == 0){

  int final = 0;
  for(i = 0; i<numnodes; ++i){
    final+=counts[i];
  }
  long final_iters = numnodes*n;


  value_pi =  ((double) (numnodes*final) / ((double) final_iters))*4;
  printf("Value of PI: %f \n",value_pi); 
  printf("Execution Time: %f \n", MPI_Wtime() - t1);
  printf("Number of Cores: %d \n", numnodes);
  }

  MPI_Finalize();
  return 0;

}
