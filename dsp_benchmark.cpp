/**
 * DSP benchmarking tool (http://kfrlib.com)
 * Copyright (C) 2016 D Levin
 * Benchmark source code is dual-licensed under MIT and GPL 2 or later
 * See LICENSE.txt for details
 */

#ifndef __OPTIMIZE__
#error dsp_benchmark can't be built in a non-optimized mode
#endif

#include "mingw_fix.h"

#ifdef TYPE_FLOAT
typedef float real;
typedef _Complex float complex;
const char* real_name = "float";
#else
typedef double real;
typedef _Complex double complex;
const char* real_name = "double";
#endif

#include "benchmark.hpp"
#include DSP_TEST
#include <cmath>
#include <string>

void test_dsp(const char* param, double time = 1.0)
{
    const tick_value correction = calibrate_correction();
    printf(">  [%9s, ", param);
    dsp_benchmark algo(param);

    algo.before();
    algo.execute();
    algo.after();

    std::vector<tick_value> measures;
    const tick_value bench_tick_start = tick();
    const time_value bench_start      = now();
    while (time_between(now(), bench_start) < time)
    {
        algo.before();
        const tick_value start = tick();
        algo.execute();
        const tick_value stop = tick();
        const tick_value diff = stop - start;
        measures.push_back(diff >= correction ? diff - correction : 0);
        algo.after();
    }
    const tick_value bench_tick_stop = tick();
    const time_value bench_stop      = now();

    const double tick_frequency =
        (long double)(bench_tick_stop - bench_tick_start) / time_between(bench_stop, bench_start);

    tick_value tick_median = get_median(measures);
    double time_median     = tick_median / tick_frequency;
    const double flops     = time_to_flops(param, time_median);

    const char* units = "'s' ";
    if (time_median < 0.000001)
    {
        units       = "'ns'";
        time_median = time_median * 1000000000.0;
    }
    else if (time_median < 0.001)
    {
        units       = "'us'";
        time_median = time_median * 1000000.0;
    }
    else if (time_median < 1.0)
    {
        units       = "'ms'";
        time_median = time_median * 1000.0;
    }

    printf("%9lld, ", tick_median);
    printf("%7g, %s, ", time_median, units);
    printf("%7g, ", flops / 1000000.0);
    printf("%8lld, ", measures.size());
    printf("%8.6f],\n", tick_frequency / 1000000000.0);
}

std::string execfile(char** argv)
{
    std::string result = argv[0];
    size_t pos         = result.find_last_of("/\\");
    result             = result.substr(pos == std::string::npos ? 0 : pos + 1);
    if (result.substr(result.size() - 4) == ".exe")
        result = result.substr(0, result.size() - 4);
    return result;
}

int main(int argc, char** argv)
{
    set_affinity();
    printf("DSP benchmarking tool\n");
    printf("Copyright (C) 2016 D Levin https://www.kfrlib.com\n");
    printf("Benchmark source code is dual-licensed under MIT and GPL 2 or later\n");
    printf("Individual algorithms have its own licenses. See its source code for details\n");
    printf("Usage:\n        %s <param1> <param2> ... <paramN>\n\n", execfile(argv).c_str());
    printf("Data type: %s\n", real_name);
    printf("Algorithm: %s\n", dsp_benchmark::name().c_str());
    printf("Built using %s\n\n", __VERSION__);

    printf(">[\n");
    for (size_t i = 1; i < argc; i++)
    {
        test_dsp(argv[i]);
    }
    printf(">]\n");
    return 0;
}
