CXX = g++ -std=c++11 -Wall -pedantic

all: scan scan_omp scan_mpi

scan: scan.cpp
	$(CXX) scan.cpp -o scan

scan_omp: scan_omp.cpp
	$(CXX) -fopenmp scan_omp.cpp -o scan_omp

scan_mpi: scan_mpi.cpp
	mpic++ scan_mpi.cpp -o scan_mpi

clean:
	rm -rf scan scan_mpi scan_omp
