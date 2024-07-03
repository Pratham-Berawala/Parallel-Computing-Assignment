#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <omp.h>

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
void bitonicMergeSerial(vector<int>& vec, int n) {
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


void bitonicSortSerial(vector<int>& vec, int size) {
    int iterations = log2(size);
    for (int i = 0; i < iterations; i++) {
        for (int j = 2; j <= size; j *= 2) {
            bitonicMergeSerial(vec, j);
        }
    }
}


void printVector(const vector<int>& vec) {
    for (int num : vec) {
        cout << num << " ";
    }
    cout << endl;
}
void validateSort(vector<int> &numbers) {
    size_t n = numbers.size();

    for (size_t i = 1; i < n; ++i) {
        if (numbers[i] < numbers[i - 1]) {
            cout<< "INVALID SORT" << endl;
            return;
        }
    }
    cout<< "VALID SORT" << endl;
}

void bitonicMerge(vector<int>& vec, int n) {
    int l;
    int k = 2;
    while (k <= n) {
        int j = k / 2;
        while (j > 0) {
            for (int i = 0; i < n; i++) {
                l = i ^ j;
                if (l > i) {
                    bool condition = ((i & k) == 0) ? (vec[i] > vec[l]) : (vec[i] < vec[l]);
                    if (condition) {
                        swap(vec[i], vec[l]);
                    }
                }
            }
            j = j / 2;
        }
        k *= 2;
    }
}


void bitonicSort(vector<int>& vec, int size) {
    int iterations = log2(size);
    int k = 2;
    for (int i = 0; i < iterations; i++) {
        #pragma omp parallel for shared(vec,size)
        for (int j = 2; j <= size; j += k) {
            bitonicMerge(vec, k);
        }
        k *= 2;
    }
}

int main(int argc, char *argv[]) {
    int size;
    int minValue, maxValue;

    cout << "Enter the size of the vector: "<<endl;
    cin >> size;

    vector<int> vec;
    fillVectorWithRandomNumbers(vec, size, 0, 10000000);

    // cout << "Randomly generated vector: ";
    // printVector(vec);

    double start, stop;
    double durationSerial, durationParallel;

    // Serial version
    vector<int> vecSerial(vec);  // Create a copy of the vector for serial sorting
    start = omp_get_wtime();
    bitonicSortSerial(vecSerial, size);
    stop = omp_get_wtime();
    durationSerial = stop - start;

    // cout << "Sorted vector (Serial): ";
    // printVector(vecSerial);

    // Parallel version
    start = omp_get_wtime();
    bitonicSort(vec, size);
    stop = omp_get_wtime();
    durationParallel = stop - start;

    // cout << "Sorted vector (Parallel): ";
    // printVector(vec);

    // Compare timings
    long long microsecondsSerial = durationSerial * 1e6;
    long long microsecondsParallel = durationParallel * 1e6;
    cout << "Time taken (Serial): " << microsecondsSerial << " microseconds" << endl;
    cout << "Time taken (OMP): " << microsecondsParallel << " microseconds" << endl;

    // Calculate speedup
    double speedup = durationSerial / durationParallel;
    cout << "Speedup: " << speedup << endl;

    validateSort(vec);
    cout << "   " <<endl;
    return 0;
}