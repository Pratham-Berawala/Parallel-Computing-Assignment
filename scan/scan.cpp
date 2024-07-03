#include <iostream>
#include <vector>
#include <random>
#include <chrono>

using namespace std;

void sequentialScan(vector<int>& input, vector<int>& output) {
    int size = input.size();
    output[0] = input[0];

    for (int i = 1; i < size; ++i) {
        output[i] = output[i - 1] + input[i];
    }
}

void fillVectorWithRandomNumbers(vector<int>& vec, int size, int minValue, int maxValue) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(minValue, maxValue);

    vec.clear();

    for (int i = 0; i < size; i++) {
        vec.push_back(dis(gen));
    }
}

void validatePrefixSum(vector<int>& originalArray, vector<int>& prefixArray) {
    size_t n = originalArray.size();
    int curr = 0;

    for (size_t i = 0; i < n; ++i) {
        curr += originalArray[i];

        if (prefixArray[i] != curr) {
            cout << "INVALID PREFIX SUM" << endl;
            return;
        }
    }

    cout << "VALID PREFIX SUM" << endl;
}

int main() {
    int size;
    cout << "Input the size of the vector: " << endl;
    cin >> size;
    vector<int> input;
    fillVectorWithRandomNumbers(input, size, 0, 100000);
    vector<int> output(input.size());

    auto start = chrono::high_resolution_clock::now();
    sequentialScan(input, output);
    auto stop = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Time taken (Seq): " << duration.count() << " microseconds" << endl;

    validatePrefixSum(input, output);
    cout <<endl;
    return 0;
}
