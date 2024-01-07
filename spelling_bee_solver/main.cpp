#include "CthSolver.hpp"

#include <chrono>
#include <iostream>

int main() {
    using namespace std;

        const auto start = chrono::high_resolution_clock::now();
        cth::prepareWordList();
        const auto end = chrono::high_resolution_clock::now();

    cout << '\n' << chrono::duration<float>(end - start).count() << "s" << endl;
}
