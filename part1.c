#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//#include "MyMPI.h"
#define MIN(a,b)  ((a)<(b)?(a):(b))
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) ( BLOCK_LOW((id)+1,p,n)-1 ) 
#define BLOCK_SIZE(id,p,n) (BLOCK_LOW( (id)+1, p, n) -  BLOCK_LOW( (id), p, n  ) )
#define BLOCK_OWNER(index,p,n) ( ( ((p)*(index)+1)-1 ) / (n) )
#define DEBUG 0
#define NORMAL 0
#define NOTWO 1
int main (int argc, char *argv[])
{
   long long int low_value=0,high_value;  
   long long int size;
   long long int n;
   long long int first;
   long long int proc0_size;
   int id,p;//p means number of processes
   long long int i,index;
   long long int count=0,global_count=0;
   char *marked; //store the array each process has
   char hostname[MPI_MAX_PROCESSOR_NAME];
  // int len;
  // int rc ;
    MPI_Init(&argc,&argv);
  /* if (rc != MPI_SUCCESS) {
	printf ("Error starting MPI program. Terminating.\n");
	MPI_Abort(MPI_COMM_WORLD, rc);
    }
*/

   //MPI_Get_processor_name(hostname, &len);
   //MPI_Init (&argc, &argv);
   MPI_Barrier(MPI_COMM_WORLD);
   MPI_Comm_rank (MPI_COMM_WORLD, &id);
   MPI_Comm_size (MPI_COMM_WORLD, &p);
   if (argc != 2) 
   {
      	if (!id) printf ("Command line: %s <m>\n", argv[0]);
      	MPI_Finalize(); 
	exit(1);
   }
#if DEBUG
   printf("umber of tasks= %d My rank= %d Running on %s\n",p,id,hostname);
#endif
   n = atoll(argv[1]);
   low_value = 2 + BLOCK_LOW(id,p,n-1); 
   high_value = 2 + BLOCK_HIGH(id,p,n-1);
   size = BLOCK_SIZE(id,p,n-1);
   proc0_size = (n-1)/p;
   if ((2 + proc0_size) < (int) sqrt((double) n)) 
   {
      if (!id) printf ("Too many processes\n");
      MPI_Finalize();
      exit(1);
   }
#if NORMAL
   marked = (char*)malloc(size*sizeof(char));
#endif 
#if NOTWO
   marked = (char*)malloc((size/2+1)*sizeof(char));
#endif

   double elapsed_time = -MPI_Wtime();
   if (marked == NULL) {
      printf ("Cannot allocate enough memory\n");
      MPI_Finalize();
      exit (2);
   }
   
   
   for (i = 0; i < size/2; i++) marked[i] = 0;
   if (!id) index = 0;
   long long int  prime = 3;
#if NORMAL
   do 
   {
      if (prime * prime > low_value)
         first = prime * prime - low_value;
      else 
      {
         if (!(low_value % prime)) first = 0;
         else first = prime - (low_value % prime);
      }
      for (i = first; i < size; i += prime) marked[i] = 1;
      if (!id)//process  0  
      {
         while (marked[++index]);
         prime = index + 2;
      }
      MPI_Bcast (&prime,  1, MPI_INT, 0, MPI_COMM_WORLD);
   } while (prime * prime <= n);
#endif
#if NOTWO
 do
   {
      if (prime * prime > low_value)
      {  
         
           first = prime * prime - low_value;//odd
           first = first /2;//important
      }
      else
      {
        if (!(low_value % prime))
	{	
		if(low_value %2 ==0)
		 first = (prime -1)/2;
		else
		 first = 0;
	}         
	else
	{
		first = prime - (low_value % prime);
		if(low_value %2==0)
		{
			if(first%2==0)
				first = first/2+(prime-1)/2;
			else
				first = (first-1)/2;
		}
		else
		{
			if(first %2==0)
				first = first /2;
			else
				first = (first-1)/2+(prime+1)/2;
		}
	}
      }
      for (i = first; i < size/2; i += prime) marked[i] = 1;
      if (!id)//process  0
      {
         while (marked[++index]);
         prime = (index*2)+ 3;
      }
      MPI_Bcast (&prime,  1, MPI_INT, 0, MPI_COMM_WORLD);
   } while (prime * prime <= n);


#endif
//----------------------------------------  
   count = 0;
   for (i = 0; i < size/2; i++)
      if (!marked[i]) count++;
   MPI_Reduce (&count, &global_count, 1, MPI_INT, MPI_SUM,0, MPI_COMM_WORLD);
   elapsed_time += MPI_Wtime();
   if (!id) {
      //printf ("%lld primes are higher tahn %lld  less than or equal to %lld\n",count,low_value, high_value);

      printf("part1\n");
      printf ("total %lld primes node:%d ",global_count+1, p/32);
      printf ("Total elapsed time: %10.6f\n", elapsed_time);
   }
   else
   {
   //    printf ("%lld primes are higher tahn %lld  less than or equal to %lld\n",count,low_value, high_value);
        
   }
   MPI_Finalize ();



   return 0;

}
