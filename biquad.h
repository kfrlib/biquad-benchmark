/**
 * DSP benchmarking tool (http://kfrlib.com)
 * Copyright (C) 2016 D Levin
 * Benchmark source code is dual-licensed under MIT and GPL 2 or later
 * See LICENSE.txt for details
 */
#pragma once

const size_t biquad_size = 4096;//1048576;

#include <cmath>

inline double time_to_flops(const char* param, double time)
{
    const long count = std::atol(param);
    return 9.0 * biquad_size * count / time;
}
