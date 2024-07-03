#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <chrono>


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

void printVector(const vector<int>& vec) {
    for (int num : vec) {
        cout << num << " ";
    }
    cout << endl;
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


int main() {
    int size;
    int minValue, maxValue;

     cout << "Enter the size of the vector: "<<endl;
    cin >> size;

    vector<int> vec;
    fillVectorWithRandomNumbers(vec, size, 0, 100000000);

    // cout << "Randomly generated vector: ";
    // printVector(vec);

    auto start = chrono::high_resolution_clock::now();
    bitonicSortSerial(vec, size);
    auto stop = chrono::high_resolution_clock::now();
    
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Time taken: " << duration.count() << " microseconds in serial" << endl;
    // cout << "Sorted vector: ";
    // printVector(vec);

    validateSort(vec);
    cout << "   " <<endl;
    return 0;
}
