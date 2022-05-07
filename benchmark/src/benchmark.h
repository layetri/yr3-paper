#pragma once

#include <vector>
#include <chrono>
#include <cmath>
#include <string.h>
#include <iostream>

using namespace std;
using namespace std::chrono;

struct Result {
    float standard_deviation;
    float average;
    long median;
    long total;
    size_t data_points;
    string name;
};

class Benchmark {
    public:
        Benchmark() = default;
        ~Benchmark() = default;
        
        void stats(vector<chrono::time_point<chrono::steady_clock>>* data, string name) {
            vector<long> diffs;
            long total = 0;

            for(int i = 0; i < data->size(); i+=2) {
                auto ts_first = data->at(i);
                auto ts_last = data->at(i+1);

                long diff = chrono::duration_cast<chrono::microseconds>(ts_last - ts_first).count();
                total += diff;

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

            results.push_back(new Result {sd, avg, median, total, diffs.size(), name});
        }

        void store_initializer(time_point<steady_clock> start, time_point<steady_clock> end, string name) {
            long diff = duration_cast<chrono::microseconds>(end - start).count();
            results.push_back(new Result {0.0, (float) diff, diff, diff, 1, name});
        }

        void print() {
            for(auto& r : results) {
                if(r->data_points > 1) {
                    cout << r->name << ": " << r->average << "us (average), " << r->total << "us (total), " << r->standard_deviation << "us (sd), " << r->median << "us (median), " << r->data_points << " (size)" << endl;
                }
            }
        }
    private:
        vector<Result*> results;
};