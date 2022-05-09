#include "benchmark.h"
#include <random>
#include <chrono>

using namespace std;

#define SCHEDULER_ROUNDS 10
namespace Scheduler {
    chrono::time_point<chrono::steady_clock> first, second;
    vector<chrono::time_point<chrono::steady_clock>> result1;
    vector<chrono::time_point<chrono::steady_clock>> result2;
    vector<chrono::time_point<chrono::steady_clock>> result3;
    vector<chrono::time_point<chrono::steady_clock>> result4;

    static void deterministic(int round, Benchmark* benchmark) {
        for(int j = 0; j < SCHEDULER_ROUNDS; j++) {
            // Aqcuire a density parameter
            auto parameters = Benchmark::getDummyPayload(j);
            // int max_grains = 64;
            vector<DummyGrain> grains;

            // Calculate the interval with which grains are triggered
            int interval = (float)parameters.grain_size / parameters.density;

            // Iterate and trigger grains
            first = steady_clock::now();
            for (int i = 0; i < (parameters.block_size - interval); i += interval) {
                grains.push_back(DummyGrain {1.0, i});
            }
            second = steady_clock::now();

            result1.push_back(first);
            result1.push_back(second);

            benchmark->quantity_test(&grains, &parameters, "schedule-deterministic-q-" + to_string(round));
            grains.clear();
        }
        benchmark->store_runtime(&result1, "schedule-deterministic-loop-" + to_string(round));
        result1.clear();
    }

    static void stochastic(int round, Benchmark* benchmark) {
        for(int j = 0; j < SCHEDULER_ROUNDS; j++) {
            // Aqcuire a set of parameters
            auto parameters = Benchmark::getDummyPayload(j);
            int max_grains = 64;
            int number_of_grains = 0;
            vector<DummyGrain> grains;

            float v = 0.0;

            first = steady_clock::now();
            for(int i = 0; i < max_grains; i++) {
                v = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                if(v < parameters.spawn_likelihood) {
                    grains.push_back(DummyGrain{v, (int) (parameters.block_size * v)});
                }
            }
            second = steady_clock::now();

            result2.push_back(first);
            result2.push_back(second);

            benchmark->quantity_test(&grains, &parameters, "schedule-start-q-" + to_string(round));
            grains.clear();
        }

        benchmark->store_runtime(&result2, "schedule-start-loop-" + to_string(round));
        result2.clear();
    }
    
    static void stochastic2(int round, Benchmark* benchmark) {
        for(int j = 0; j < SCHEDULER_ROUNDS; j++) {
            // Aqcuire a set of parameters
            auto parameters = Benchmark::getDummyPayload(j);
            int max_grains = floor(64 * parameters.spawn_likelihood);
            int number_of_grains = 0;
            vector<DummyGrain> grains;

            float v = 0.0;
            bool seed = false;
            int wait = 0;

            random_device rd;
            mt19937 gen(rd());
            uniform_real_distribution<> dist(0, 1);

            first = steady_clock::now();
            for(int i = 0; i < parameters.block_size; i++) {
                
                    // v = dist(gen);
                    v = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                    seed = v < parameters.spawn_likelihood;
                    if(max_grains > 0 && seed) {
                        grains.push_back(DummyGrain {v, i});
                        max_grains--;
                        i += parameters.minimum_interval % (parameters.block_size - i);
                        
                    }
            }
            second = steady_clock::now();

            result3.push_back(first);
            result3.push_back(second);

            benchmark->quantity_test(&grains, &parameters, "schedule-chance-q-" + to_string(round));
            grains.clear();
        }

        benchmark->store_runtime(&result3, "schedule-chance-loop-" + to_string(round));
        result2.clear();
    }

    static void dividedown(int round, Benchmark* benchmark) {
        vector<DummyGrain> grains;
        float v = 0.0;
        bool seed = false;

        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dist(0, 1);

        for(int j = 0; j < SCHEDULER_ROUNDS; j++) {
            // Aqcuire a set of parameters
            auto parameters = Benchmark::getDummyPayload(j);
            int max_grains = floor(64 * parameters.spawn_likelihood);
            int number_of_grains = 0;

            int interval = (float) parameters.block_size / (float) max_grains;

            // Iterate and trigger grains
            first = steady_clock::now();
            while(max_grains > 1) {
                interval = (float) parameters.block_size / (float) max_grains;

                if(interval > parameters.minimum_interval) {
                    for (int i = 0; i < (parameters.block_size - interval); i += interval) {
                        if(max_grains > 0) {
                            v = dist(gen);
                            seed = v < parameters.spawn_likelihood;
                            if (seed) {
                                grains.push_back(DummyGrain {v, i});
                                max_grains--;
                            }
                        }
                    }
                } else {
                    max_grains = 0;
                }
            }
            second = steady_clock::now();

            result4.push_back(first);
            result4.push_back(second);

            benchmark->quantity_test(&grains, &parameters, "schedule-dividedown-q-" + to_string(round));
            grains.clear();
        }

        benchmark->store_runtime(&result4, "schedule-dividedown-loop-" + to_string(round));
        result4.clear();
    }
}