all:
	mpicc part1.c -o run_part1
	mpicc part2.c -o run_part2
	mpicc part3.c -o run_part3

clean:
	rm run.e* run.o*  
