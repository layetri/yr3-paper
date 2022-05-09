#pragma once

#include <vector>
#include <chrono>
#include <cmath>
#include <string.h>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>

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

struct QuantityResult {
    float start_sd;
    float start_avg;
    float norm_start_avg;
    int start_median;

    int interval_min;
    int interval_max;

    int cover_min;
    int cover_max;
    float coverage;

    float input_density;
    float output_density;

    float density_diff;
    int block_size;

    int exp_grains;
    int act_grains;
    float exp_density;
    float act_density;

    size_t data_points;
    string name;
};

struct DummyGrain {
    float seed = 0.0;
    int start = 100;
};

struct RandomParameterContainer {
    int grain_size = 500;
    int block_size = 44100;
    int minimum_interval = (block_size / 65);
    float density = 0.75;
    float spawn_likelihood = 0.65;
    // float density = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    // float spawn_likelihood = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
};

class Benchmark {
    public:
        Benchmark() = default;
        ~Benchmark() = default;

        static RandomParameterContainer getDummyPayload(int round) {
            auto p = RandomParameterContainer();
            p.density = (float) round / 100.0f;
            p.spawn_likelihood = (float) round / 100.0f;
            return p;
        }

        static float standardDeviation(vector<long>* data, float average) {
            float total_deviation = 0.0;
            for (auto &d : *data) {
                total_deviation += ((float)d - average) * ((float)d - average);
            }

            total_deviation = total_deviation / data->size();
            return sqrt(total_deviation);
        }

        void quantity_test(vector<DummyGrain>* pool, RandomParameterContainer* parameters, string name) {
            if(pool->size() > 0) {
                // Print information about grain density
                float density_diff = 0.0;
                float exp_density = 0.0, act_density = 0.0;
                int start_min = parameters->block_size;
                int start_max = 0;
                int int_min = parameters->block_size;
                int int_max = 0;

                int expected_n_grains = 0;
                int actual_n_grains = pool->size();

                float lin = 0.0;
                auto density = (float) (pool->size() * parameters->grain_size) / (float) parameters->block_size;
                
                if (name.find("deterministic") != string::npos) {    
                    expected_n_grains = (float) parameters->block_size / (float) (parameters->grain_size / parameters->density);
                    exp_density = density;
                    act_density = parameters->density;
                    density_diff = density - parameters->density;

                    lin = parameters->density;
                } else {
                    lin = parameters->spawn_likelihood;
                }

                int prev = 0;
                long total_start_diff = 0;
                vector<long> start_diff;

                sort(pool->begin(), pool->end(), [](DummyGrain a, DummyGrain b) {
                    return a.start < b.start;
                });

                for(auto& g : *pool) {
                    auto diff = g.start - prev;
                    prev = g.start + parameters->grain_size;
                    if(pool->size() == 1 || diff != 0) {
                        start_diff.push_back(diff);
                        total_start_diff += diff;

                        if(g.start < start_min) {
                            start_min = g.start;
                        }
                        if(g.start + parameters->grain_size > start_max) {
                            start_max = g.start + parameters->grain_size;
                        }
                        if(diff > int_max) {
                            int_max = diff;
                        }
                        if(diff < int_min) {
                            int_min = diff;
                        }
                    }
                }

                auto coverage = (float) (start_max - start_min) / (float) parameters->block_size;

                auto average_start_diff = (float) total_start_diff / (float) start_diff.size();
                auto median_start_diff = start_diff[floor((float)start_diff.size() / 2.0)];
                auto sd_start_diff = standardDeviation(&start_diff, average_start_diff);
                auto normalized_start_diff = (average_start_diff / (float) parameters->block_size) * 1000;

                quantity_results.push_back(new QuantityResult {
                    sd_start_diff,
                    average_start_diff,
                    normalized_start_diff,
                    (int) median_start_diff,
                    start_min,
                    start_max,
                    int_min,
                    int_max,
                    coverage,
                    lin,
                    density,
                    density_diff,
                    parameters->block_size,
                    expected_n_grains,
                    actual_n_grains,
                    exp_density,
                    act_density,
                    start_diff.size(),
                    name
                });
            }
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

            auto sd = standardDeviation(&diffs, avg);

            results.push_back(new Result {sd, avg, min, max, median, total, diffs.size(), name});
        }

        void store_initializer(time_point<steady_clock> start, time_point<steady_clock> end, string name) {
            long diff = duration_cast<chrono::nanoseconds>(end - start).count();
            results.push_back(new Result {0.0, (float) diff, diff, diff, diff, diff, 1, name});
        }

        void print() {
            ofstream resfile;
            // resfile.open(to_string(rand()) + ".txt");
            resfile.open("results/latest.txt");
            for(auto& r : results) {
                if(r->data_points > 1) {
                    cout << r->name << ": " << r->average << "ns (average), " << r->standard_deviation << "ns (sd), " << r->median << "ns (median), " << r->total << "ns (total), " << r->data_points << " (size)" << endl;
                    resfile << r->name << ", " << r->average << ", " << r->min << ", " << r->max << ", " << r->standard_deviation << ", " << r->median << ", " << r->total << ", " << r->data_points << "\n";
                }
            }

            cout << endl;

            for(auto& r : results) {
                if(r->data_points == 1) {
                    resfile << r->name << ", " << r->average << ", " << r->min << ", " << r->max << ", " << r->standard_deviation << ", " << r->median << ", " << r->total << ", " << r->data_points << "\n";
                }
            }
            
            for(auto& q : quantity_results) {
                cout << q->name << ": " <<
                    q->start_avg << " (average interval)[1], "<<
                    q->norm_start_avg << " (norm interval)[2], " <<
                    q->coverage * 100.0 << "% (coverage)[3], " <<
                    q->output_density << "(" << q->input_density << ") (linearity)[4], " <<
                    q->start_sd << " (sd interval)[5] [" <<
                    q->interval_min << "," << q->interval_max << "], " <<
                    q->exp_grains << " (n expected)[6], " <<
                    q->act_grains << " (n actual)[7], " <<
                    q->density_diff << " (density diff)[8] [" <<
                    q->exp_density << ", " << q->act_density << "], " <<
                    q->block_size << " (size)[-]" << endl;

                resfile << q->name << ", " << q->start_avg << ", " << q->norm_start_avg << ", " << (q->coverage > 1.0 ? 100 : (q->coverage * 100)) << ", " << q->input_density << ", " << q->output_density << ", " << q->start_sd << ", " << q->exp_grains << ", " << q->act_grains << ", " << q->density_diff << ", " << q->data_points << "\n";
            }
            resfile.close();
        }
    private:
        vector<Result*> results;
        vector<QuantityResult*> quantity_results;
};