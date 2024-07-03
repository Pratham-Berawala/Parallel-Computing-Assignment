all: bitonic bitonic_omp bitonic_mpi

bitonic: bitonic.cpp
	g++ bitonic.cpp -o bitonic

bitonic_omp: bitonic_omp.cpp
	g++ -fopenmp bitonic_omp.cpp -o bitonic_omp

bitonic_mpi: bitonic_mpi.cpp
	mpic++ bitonic_mpi.cpp -o bitonic_mpi


