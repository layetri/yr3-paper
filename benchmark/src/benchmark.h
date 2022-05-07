#pragma once

#include <vector>
#include <chrono>
#include <cmath>
#include <string.h>
#include <iostream>
#include <fstream>
#include <random>

using namespace std;
using namespace std::chrono;

struct Result {
    float standard_deviation;
    float average;
    long min;
    long max;
    long median;
    long total;
    size_t data_points;
    string name;
};

struct DummyGrain {
    int start = 100;
};

struct RandomParameterContainer {
    int grain_size = rand() % 1000;
    int block_size = rand() % 44100;
    float density = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float spawn_likelihood = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
};

class Benchmark {
    public:
        Benchmark() = default;
        ~Benchmark() = default;

        static RandomParameterContainer getDummyPayload() {
            return RandomParameterContainer();
        }

        void quantity_test(vector<DummyGrain>* pool, string name) {
            // Print information about grain density

            // Print information about parameters
        }
        
        void store_runtime(vector<chrono::time_point<chrono::steady_clock>>* data, string name) {
            vector<long> diffs;
            long total = 0;
            long min = 10000000;
            long max = 0;

            for(int i = 0; i < data->size(); i+=2) {
                auto ts_first = data->at(i);
                auto ts_last = data->at(i+1);

                long diff = chrono::duration_cast<chrono::nanoseconds>(ts_last - ts_first).count();
                total += diff;

                if(diff > max) {
                    max = diff;
                }
                if(diff < min) {
                    min = diff;
                }

                diffs.push_back(diff);
            }

            float avg = (float) total / (float) diffs.size();
            auto median = diffs[floor((float) diffs.size() / 2.0)];

            float total_deviation = 0.0;

            for(auto& d : diffs) {
                total_deviation += ((float) d - avg) * ((float) d - avg);
            }

            total_deviation = total_deviation / diffs.size();
            auto sd = sqrt(total_deviation);

            results.push_back(new Result {sd, avg, min, max, median, total, diffs.size(), name});
        }

        void store_initializer(time_point<steady_clock> start, time_point<steady_clock> end, string name) {
            long diff = duration_cast<chrono::nanoseconds>(end - start).count();
            results.push_back(new Result {0.0, (float) diff, diff, diff, diff, diff, 1, name});
        }

        void print() {
            ofstream resfile;
            srand(time(NULL));
            // resfile.open(to_string(rand()) + ".txt");
            resfile.open("results/latest.txt");
            for(auto& r : results) {
                if(r->data_points > 1) {
                    cout << r->name << ": " << r->average << "ns (average), " << r->standard_deviation << "ns (sd), " << r->median << "ns (median), " << r->total << "ns (total), " << r->data_points << " (size)" << endl;
                    resfile << r->name << ", " << r->average << ", " << r->min << ", " << r->max << ", " << r->standard_deviation << ", " << r->median << ", " << r->total << ", " << r->data_points << "\n";
                }
            }

            for(auto& r : results) {
                if(r->data_points == 1) {
                    resfile << r->name << ", " << r->average << ", " << r->min << ", " << r->max << ", " << r->standard_deviation << ", " << r->median << ", " << r->total << ", " << r->data_points << "\n";
                }
            }
            resfile.close();
        }
    private:
        vector<Result*> results;
};