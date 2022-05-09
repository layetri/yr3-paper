#include <chrono>
#include <filesystem>
#include <iostream>

#include "envelope.h"
#include "scheduler.h"
#include "benchmark.h"

#define ENV_RNDS 1000
#define ROUNDS 100

using namespace std;

int main() {
    srand(time(NULL));
    Benchmark benchmark;

    // Do Envelope benchmark
    for(int i = 0; i < ENV_RNDS; i++) {
        Envelope::preprocess(i, &benchmark);

        Envelope::lookup(i, &benchmark);

        Envelope::realtime(i, &benchmark);
    }

    // Do Scheduler benchmark
    for(int i = 0; i < ROUNDS; i++) {
        Scheduler::deterministic(i, &benchmark);

        Scheduler::stochastic(i, &benchmark);

        Scheduler::stochastic2(i, &benchmark);

        Scheduler::dividedown(i, &benchmark);
    }

    benchmark.print();

    exit(0);
}