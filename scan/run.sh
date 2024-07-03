#!/bin/bash
echo =======================================
echo ---------Run Make----------
make
echo ---------Benchmark started----------
exponent=(32768 65536 131072)
nProcs=(1 2 4)

for pe in "${nProcs[@]}"
do
    echo ---------Working on $pe PEs----------
    for exp in "${exponent[@]}"
    do
        echo ${exp} > input.txt  # Clear the file before writing the new value
        echo =======================================
        echo "Testing array size ${exp}"
        ./scan < input.txt
        # export OMP_NUM_THREADS=${pe}
        ./scan_omp < input.txt
        mpirun -np ${pe} ./scan_mpi < input.txt
        echo =======================================
    done
    echo -------------------------------------
done
echo ---------Run Make Clean----------
make clean
echo ---------END----------
echo =======================================
