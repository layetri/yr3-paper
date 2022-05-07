#include "benchmark.h"
#include <random>

#define SCHEDULER_ROUNDS 1000
namespace Scheduler {
    static void deterministic(int round, Benchmark* benchmark) {
        // Aqcuire a density parameter
        auto density = Benchmark::getDummyPayload().density;
        int max_grains = 64;

    }

    static void stochastic(int round, Benchmark* benchmark) {
        for(int j = 0; j < SCHEDULER_ROUNDS; j++) {
            // Aqcuire a set of parameters
            auto parameters = Benchmark::getDummyPayload();
            int max_grains = 64;
            int number_of_grains = 0;
            DummyGrain* grain_list[max_grains];

            for(int i = 0; i < parameters.block_size; i++) {
                auto seed = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) > parameters.spawn_likelihood;
                if(max_grains > 0 && seed) {
                    grain_list[number_of_grains] = new DummyGrain;
                }
            }
        }
    }
}