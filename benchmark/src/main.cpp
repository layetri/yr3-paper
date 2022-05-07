#include <chrono>
#include <filesystem>
#include <iostream>

#include "envelope.h"
#include "scheduler.h"
#include "benchmark.h"

#define ROUNDS 1000

using namespace std;

int main() {
    Benchmark benchmark;

    // Do Envelope benchmark
    for(int i = 0; i < ROUNDS; i++) {
        Envelope::preprocess(i, &benchmark);

        Envelope::lookup(i, &benchmark);

        Envelope::realtime(i, &benchmark);
    }

    // // Do Scheduler benchmark
    // for(int i = 0; i < ROUNDS; i++) {
    //     Scheduler::deterministic(i, &benchmark);
    // }

    // for(int i = 0; i < ROUNDS; i++) {
    //     Scheduler::stochastic(i, &benchmark);
    // }

    benchmark.print();

    exit(0);
}