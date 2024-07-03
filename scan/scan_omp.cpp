#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
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

void printVector(const vector<int>& vec) {
    for (int num : vec) {
        cout << num << " ";
    }
    cout << endl;
}

void parallelPrefix(vector<int>& input, int size) {
    int last = input[size - 1];
    int itera = log2(size);
    // Upsweep
    for (int d = 0; d < itera; d++) {
        int incre = pow(2, d + 1);
        #pragma omp parallel for shared(size,incre) schedule(auto) 
        for (int i = 0; i < size - 1; i += incre) {
            input[i + incre - 1] = input[i + pow(2, d) - 1] + input[i + incre - 1];
        }
    }
    // printVector(input);
    input[size - 1] = 0;

    // Downsweep
    for (int d = itera - 1; d >= 0; d--) {
        int incre = pow(2, d + 1);
        #pragma omp parallel for shared(size,incre) schedule(auto)
        for (int i = 0; i < size; i += incre) {
            int temp = input[i + incre / 2 - 1];
            input[i + incre / 2 - 1] = input[i + incre - 1];
            input[i + incre - 1] = temp + input[i + incre - 1];
        }
    }
    rotate(input.begin(), input.begin() + 1, input.end());
    input[size - 1] = last + input[size - 2];
    
}

void validatePrefixSum(vector<int>& originalArray, vector<int>& prefixArray) {
    size_t n = originalArray.size();
    for(size_t i = 1; i < n;i++){
        originalArray[i] += originalArray[i-1];
    }
    for (size_t i = 0; i < n; ++i) {
        if (prefixArray[i] != originalArray[i]) {
            cout << "INVALID PREFIX SUM" << endl;
            return;
        }
    }

    cout << "VALID PREFIX SUM" << endl;
}
void sequentialScan(vector<int>& input, vector<int>& output) {
    int size = input.size();
    output[0] = input[0];

    for (int i = 1; i < size; ++i) {
        output[i] = output[i - 1] + input[i];
    }
}


int main() {
    int size;
    // int threads;
    cout << "Input the size: ";
    cin >> size;
    cout << endl;

    // cout << "Input the thread number: ";
    // cin >> threads;

    // omp_set_num_threads(threads);

    vector<int> input(size);
    fillVectorWithRandomNumbers(input, size, 0, 100);

    vector<int> valid = input;
    vector<int> output(size);
    
    auto start_parallel = omp_get_wtime();
    parallelPrefix(valid, size);
    auto stop_parallel = omp_get_wtime();

    double duration_parallel = stop_parallel - start_parallel;
    double microseconds_parallel = duration_parallel * 1e6;
    cout << "Time taken (OMP): " << microseconds_parallel << " microseconds" << endl;

    auto start = omp_get_wtime();
    sequentialScan(input,output);
    auto stop = omp_get_wtime();

    auto duration = stop - start;
    double microseconds = duration * 1e6;
    cout << "Time taken (Seq): " << microseconds << " microseconds" << endl;

    double speedup = microseconds/microseconds_parallel;
    cout << "Speedup: " << speedup << endl;

    validatePrefixSum(input,valid);
    cout << "  " << endl;
    return 0;
}