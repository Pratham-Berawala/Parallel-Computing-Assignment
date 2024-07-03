#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <chrono>
#include <mpi.h>

using namespace std;

void fillVectorWithRandomNumbers(vector<int>& vec, int size, int minValue, int maxValue) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(minValue, maxValue);

    vec.clear();

    for (int i = 0; i < size; i++) {
        vec.push_back(dis(gen));
    }
}

void validateSort(const vector<int>& numbers) {
    size_t n = numbers.size();

    for (size_t i = 1; i < n; ++i) {
        if (numbers[i] < numbers[i - 1]) {
            cout << "INVALID SORT" << endl;
            return;
        }
    }
    cout << "VALID SORT" << endl;
}

void printVector(const vector<int>& vec) {
    for (int num : vec) {
        cout << num << " ";
    }
    cout << endl;
}

void bitonicMerge(vector<int>& vec, int n) {
    int l;
    for (int k = 2; k <= n; k *= 2) {
        for (int j = k / 2; j > 0; j = j / 2) {
            for (int i = 0; i < n; i++) {
                l = i ^ j;
                if (l > i) {
                    bool condition = ((i & k) == 0) ? (vec[i] > vec[l]) : (vec[i] < vec[l]);
                    if (condition) {
                        swap(vec[i], vec[l]);
                    }
                }
            }
        }
    }
}

void bitonicSort(vector<int>& vec, int size) {
    int iterations = log2(size);
    for (int i = 0; i < iterations; i++) {
        for (int j = 2; j <= size; j *= 2) {
            bitonicMerge(vec, j);
        }
    }
}

int main(int argc, char** argv) {
    int size, rank, numProcesses;
    int minValue, maxValue;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    vector<int> vec; // Declare the vec variable

    if (rank == 0) {
        cout << "Enter the size of the vector: "<<endl;
        cin >> size;
        fillVectorWithRandomNumbers(vec, size, 0, 100000000);
    }

    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Determine the portion of the vector to be processed by each process
    int localSize = size / numProcesses;
    vector<int> localVec(localSize);

    // Scatter the vector data among processes
    MPI_Scatter(vec.data(), localSize, MPI_INT, localVec.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);

    // Sort the local vector using MPI
    auto mpiStart = chrono::high_resolution_clock::now();
    bitonicSort(localVec, localSize);
    auto mpiStop = chrono::high_resolution_clock::now();

    // Gather the sorted subvectors from all processes
    MPI_Gather(localVec.data(), localSize, MPI_INT, vec.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Merge the sorted subvectors
        int subSize = localSize;
        for (int i = 1; i < numProcesses; i++) {
            bitonicMerge(vec, subSize);
            subSize *= 2;
        }

        // Serial bitonic sort
        vector<int> serialVec = vec; // Copy the vector for serial sorting
        auto serialStart = chrono::high_resolution_clock::now();
        bitonicSort(serialVec, size);
        auto serialStop = chrono::high_resolution_clock::now();

        // Validate the sorted vectors
        
        // Print the execution times
        auto mpiDuration = chrono::duration_cast<chrono::microseconds>(mpiStop - mpiStart);
        cout << "Time taken (MPI): " << mpiDuration.count() << " microseconds" << endl;

        auto serialDuration = chrono::duration_cast<chrono::microseconds>(serialStop - serialStart);
        cout << "Time taken (Serial): " << serialDuration.count() << " microseconds" << endl;

        validateSort(serialVec);

        // Calculate speedup
        double speedup = static_cast<double>(serialDuration.count()) / mpiDuration.count();
        cout << "Speedup: " << speedup << endl;
        cout << "   " <<endl;

    }

    MPI_Finalize();

    return 0;
}
