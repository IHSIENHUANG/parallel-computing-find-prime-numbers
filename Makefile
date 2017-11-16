all:
	mpicc part1.c -o run_part1
	mpicc part2.c -o run_part2
clean:
	rm run.e* run.o* 
