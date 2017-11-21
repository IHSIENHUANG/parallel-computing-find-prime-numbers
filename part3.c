#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include "MyMPI.h"
#define MIN(a,b)  ((a)<(b)?(a):(b))
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) ( BLOCK_LOW((id)+1,p,n)-1 ) 
#define BLOCK_SIZE(id,p,n) (BLOCK_LOW( (id)+1, p, n) -  BLOCK_LOW( (id), p, n  ) )
#define BLOCK_OWNER(index,p,n) ( ( ((p)*(index)+1)-1 ) / (n) )
#define DEBUG 0
#define NORMAL 0
#define NOTWO 0//this is the version allocate memory without multiple of 2 
#define NOBROWCAST 1//this is the NOWTW + skip browcast
long long int find_first(long long int prime,long long int low_value)
{
      long long int first=0;
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
      return first;


}
int main (int argc, char *argv[])
{
   long long int low_value=0,high_value;  
   long long int size;
   long long int n;
   long long int first[4];
   long long int proc0_size;
   int id,p;//p means number of processes
   register long long int i,index;
   long long int count=0,global_count=0;
   char *marked; //store the array each process has
   char *marked_prime;// each process own its seiving prime
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
   //printf("node  is %d \n",p);
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
   long long int test  = 100010;
   marked = (char*)malloc((size/2+1)*sizeof(char));
   marked_prime = (char*)malloc((test)*sizeof(char));
   if (marked == NULL|| marked_prime ==NULL) {
      printf ("Cannot allocate enough memory\n");
      MPI_Finalize();
      exit (2);
   }
   
   for (i = 0; i < size/2+1; i++) marked[i] = 0;
   for (i = 0 ;i < test;i++) marked_prime[i]=0;
   //memset(marked,0,(size/2+1) );
   //memset(marked_prime,0,test);

   index = 5;
    int round;
    int j ;

   double elapsed_time = -MPI_Wtime();
   int cache = 1;
   long long int  prime[5];
   prime[0] = 3;
   prime[1] = 5;
   prime[2] = 7;
   prime[3] = 11;
   prime[4] =13;
   
   long long int first_sezing[5];//for the array of small array 
  do
  {  
     if(cache ==0)//a prime per round 
     {

        first[0] = find_first(prime[0],low_value);
	first_sezing[0] = prime[0]*prime[0]-2;
	first_sezing[0] /=2;
      for (i = first[0]; i < size/2; i += prime[0]) marked[i] = 1;
      for (i = first_sezing[0]; i < test; i+=prime[0]) marked_prime[i] = 1;
 
      while (marked_prime[++index]);
         prime[0] = (index*2)+ 3;
     }
     else//four prime per round
     {
	for(i = 0 ; i < 5;i++)
	{
		first[i] = find_first(prime[i],low_value);
		first_sezing[i]=prime[i]*prime[i]-2;
		first_sezing[i]/=2;
	}     	   
 	for (i = 0; i < size/2; i += 10000)
	{
	    	for(round =0;round < 5 ;round ++)
	    	{
			for(j=first[round];j<MIN(size/2,i+10000);j+=prime[round])
			{
				marked[j]=1;			
			}
			first[round] = j;	
		}
	}
        for (i = first_sezing[0]; i < test; i+=prime[0]) marked_prime[i] = 1;
	for (i = first_sezing[1]; i < test; i+=prime[1]) marked_prime[i] = 1;
	for (i = first_sezing[2]; i < test; i+=prime[2]) marked_prime[i] = 1;
	for (i = first_sezing[3]; i < test; i+=prime[3]) marked_prime[i] = 1;
        for (i = first_sezing[4]; i < test; i+=prime[4]) marked_prime[i] = 1;
        if(prime[0]>2000) 
	{
	 	cache= 0;
		 while (marked_prime[++index]);
                 prime[0] = (index*2)+ 3;
	}
	if(cache ==1)
	{

		for(i = 0 ; i< 5 ; i++)
         	{
                    while (marked_prime[++index]);
                    prime[i] = (index*2)+ 3;
        	}	
	}
     }
     // if(id==0) printf("first num = %lld\n",first_sezing); 
   } while (prime[0] * prime[0] <= n);

//----------------------------------------  
   count = 0;
   for (i = 0; i < size/2; i++)
      if (!marked[i]) count++;
   MPI_Reduce (&count, &global_count, 1, MPI_INT, MPI_SUM,0, MPI_COMM_WORLD);
   elapsed_time += MPI_Wtime();
   if (!id) {
   //   printf ("%lld primes are higher tahn %lld  less than or equal to %lld\n",count,low_value, high_value);
      if(p/32==1) printf("\npart3:\n");
      printf ("total %lld primes node:%d  ",global_count+1, p/32);
      printf ("Total elapsed time: %10.6f\n", elapsed_time);
   }
   else
   {
  //     printf ("%lld primes are higher tahn %lld  less than or equal to %lld\n",count,low_value, high_value);
        
   }
   MPI_Finalize ();



   return 0;

}
