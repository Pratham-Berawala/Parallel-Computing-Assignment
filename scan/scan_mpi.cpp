#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <mpi.h>

using namespace std;

double microseconds_serial; // Global variable

void sequentialScan(vector<int>& input, vector<int>& output);

void validatePrefixSum(vector<int>& originalArray, vector<int>& prefixArray) {
    size_t n = originalArray.size();
    vector<int> prefixSum(n);
    sequentialScan(originalArray, prefixSum);

    for (size_t i = 0; i < n; ++i) {
        if (prefixArray[i] != prefixSum[i]) {
            cout << "INVALID PREFIX SUM" << endl;
            return;
        }
    }

    cout << "VALID PREFIX SUM" << endl;
}

void fillVectorWithRandomNumbers(vector<int>& input, int size, int minValue, int maxValue) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(minValue, maxValue);

    for (int i = 0; i < size; i++) {
        input.push_back(dis(gen));
    }
}

void sequentialScan(vector<int>& input, vector<int>& output) {
    int size = input.size();
    output[0] = input[0];

    for (int i = 1; i < size; ++i) {
        output[i] = output[i - 1] + input[i];
    }
}

void printVector(const vector<int>& vec) {
    for (int num : vec) {
        cout << num << " ";
    }
    cout << endl;
}

int log2_int(int x) {
    int result = 0;
    while (x >>= 1) {
        result++;
    }
    return result;
}

void parallelPrefix(vector<int>& localArray, int localSize, int rank, int worldSize) {
    for (int i = 1; i < localSize; ++i) {
        localArray[i] = localArray[i - 1] + localArray[i];
    }
    int last = localArray[localSize - 1];

    vector<int> down(worldSize);
    MPI_Gather(&last, 1, MPI_INT, down.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Upsweep
        int d_max = log2_int(worldSize);
        for (int d = 0; d < d_max; d++) {
            int incre = pow(2, d + 1);
            for (int i = 0; i < worldSize - 1; i += incre) {
                down[i + incre - 1] = down[i + pow(2, d) - 1] + down[i + incre - 1];
            }
        }

        down[worldSize - 1] = 0;

        // Downsweep
        for (int d = d_max - 1; d >= 0; d--) {
            int incre = pow(2, d + 1);
            for (int i = 0; i < worldSize; i += incre) {
                int temp = down[i + incre / 2 - 1];
                down[i + incre / 2 - 1] = down[i + incre - 1];
                down[i + incre - 1] = temp + down[i + incre - 1];
            }
        }
    }

    int addTo = 0;
    MPI_Scatter(down.data(), 1, MPI_INT, &addTo, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < localSize; ++i) {
        localArray[i] += addTo;
    }
}

int main(int argc, char** argv) {
    int rank, worldSize;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    int arraySize;
    if (rank == 0) {
        cout << "Enter the size of the array: "<<endl;
        cin >> arraySize;
    }
    MPI_Bcast(&arraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int> inputArray;
    vector<int> outputArray;

    int localSize = arraySize / worldSize;
    vector<int> localArray(localSize);

    if (rank == 0) {
        fillVectorWithRandomNumbers(inputArray, arraySize, 1, 10);
        outputArray.resize(arraySize);
    }

    double start, end;

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    MPI_Scatter(inputArray.data(), localSize, MPI_INT, localArray.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);

    parallelPrefix(localArray, localSize, rank, worldSize);

    MPI_Gather(localArray.data(), localSize, MPI_INT, outputArray.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    if (rank == 0) {
        double microseconds_parallel = (end - start) * 1000000.0;
        cout << "Time taken (MPI): " << microseconds_parallel << " microseconds" << endl;

        vector<int> serialOutput(arraySize);
        start = MPI_Wtime();
        sequentialScan(inputArray, serialOutput);
        end = MPI_Wtime();
        microseconds_serial = (end - start) * 1000000.0;
        cout << "Time taken (Seq): " << microseconds_serial << " microseconds" << endl;

        validatePrefixSum(inputArray, outputArray);

        double speedup = microseconds_serial / microseconds_parallel;
        cout << "Speedup: " << speedup << endl;
        cout << "  " << endl;
    }

    MPI_Finalize();

    return 0;
}
