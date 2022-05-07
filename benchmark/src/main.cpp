#include <chrono>
#include <filesystem>
#include <iostream>

#include "envelope.h"
#include "benchmark.h"

#define ROUNDS 10

using namespace std;

int main() {
    Benchmark benchmark;

    for(int i = 0; i < ROUNDS; i++) {
        Envelope::preprocess(i, &benchmark);

        cout << "ENV PRE - Round " << i << " complete" << endl;
    }

    for(int i = 0; i < ROUNDS; i++) {
        Envelope::lookup(i, &benchmark);

        cout << "ENV LUT - Round " << i << " complete" << endl;
    }

    for(int i = 0; i < ROUNDS; i++) {
        Envelope::realtime(i, &benchmark);

        cout << "ENV RT - Round " << i << " complete" << endl;
    }

    benchmark.print();

    exit(0);
}