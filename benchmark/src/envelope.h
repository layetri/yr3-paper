#pragma once

#include <chrono>
#include <vector>
#include <random>

#include "benchmark.h"
#include "lut.h"

#define ENV_ROUNDS 4096
#define ENV_SHAPE 0.5

using namespace std;

// How to envelope?
namespace Envelope {
    float sample[4096] = {};

    float test1[4096];
    float test2[4096];
    float test3[4096];
    
    chrono::time_point<chrono::steady_clock> first, second;
    vector<chrono::time_point<chrono::steady_clock>> result1;
    vector<chrono::time_point<chrono::steady_clock>> result2;
    vector<chrono::time_point<chrono::steady_clock>> result3;

    // This is how I would approach it
    static void preprocess(int round, Benchmark* benchmark) {
        first = chrono::steady_clock::now();
        // Render process table
        float envelope[ENV_ROUNDS];
        float tmp;
        srand(time(NULL));
        float shape = (float) rand() / (float) RAND_MAX;

        // Do this when a trigger is received or key is struck, to assure the latest parameter settings are used
        for(int i = 0; i < ENV_ROUNDS; i++) {
            // Render a parabolic curve
            tmp = shape * (float)(-1.0 * i * i + ENV_ROUNDS * i);
            // Clip the values to [0.0, 1.0] and put them in place
            envelope[i] = (tmp >= 1.0) * 1.0 + (tmp < 1.0) * tmp;
        }
        second = chrono::steady_clock::now();
        benchmark->store_initializer(first, second, "envelope-preprocess-init");
        
        // Then do this on rendering
        for(int i = 0; i < ENV_ROUNDS; i++) {
            first = chrono::steady_clock::now();

            // Look up from process table
            // -> No need for interpolating or stretching, since the LUT is calculated exactly for these parameters
            test1[i] = sample[i] * envelope[i];

            second = chrono::steady_clock::now();
            result1.push_back(first);
            result1.push_back(second);
        }

        benchmark->store_runtime(&result1, "envelope-preprocess-render-" + to_string(round));
        result1.clear();
    };

    // This is INSPIRED BY how MI Clouds does it (because I for the life of me cannot figure out the complete code)
    static void lookup(int round, Benchmark* benchmark) {
        first = chrono::steady_clock::now();

        // Calculate scaling factor
        auto scale_factor = (sizeof(ENVELOPE_LUT) / sizeof(ENVELOPE_LUT[0])) / ENV_ROUNDS;
        // Get filtering argument
        float previous = ENVELOPE_LUT[0];

        int ptr = 0;
        float tmp = 0.0;

        second = chrono::steady_clock::now();
        benchmark->store_initializer(first, second, "envelope-lookup-init");

        // Do this on rendering
        for(int i = 0; i < ENV_ROUNDS; i++) {
            first = chrono::steady_clock::now();

            // Scale to fit
            ptr = floor(scale_factor * i);

            // Look up from lookup table
            tmp = 0.5 * (ENVELOPE_LUT[ptr] + previous);

            // Apply to test sample
            test2[i] = tmp * sample[i];
            previous = ENVELOPE_LUT[ptr];

            second = chrono::steady_clock::now();
            result2.push_back(first);
            result2.push_back(second);
        }

        benchmark->store_runtime(&result2, "envelope-lookup-render-" + to_string(round));
        result2.clear();
    };

    // This is how Argotlunar does it
    static void realtime(int round, Benchmark* benchmark) {
        first = chrono::steady_clock::now();
        
        // Initialize curve: we're using a parabolic curve for all examples
        float env_amp = 0.0f;
        float d = 1.0f / ENV_ROUNDS;
        float d2 = d * d;
        float slope = 4.0f * 0.8 * (d - d2);
        float curve = -8.0f * 0.8 * d2;

        second = chrono::steady_clock::now();
        benchmark->store_initializer(first, second, "envelope-realtime-init");

        // Do this on rendering
        for (int i = 0; i < ENV_ROUNDS; i++) {
            first = chrono::steady_clock::now();

            env_amp += slope;
            slope += curve;
            test3[i] = sample[i] * env_amp;

            second = chrono::steady_clock::now();
            result3.push_back(first);
            result3.push_back(second);
        }

        benchmark->store_runtime(&result3, "envelope-realtime-render-" + to_string(round));
        result3.clear();
    };
};