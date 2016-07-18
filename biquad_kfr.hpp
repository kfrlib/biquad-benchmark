/**
 * FFT bencmarking tool (http://kfrlib.com)
 * Copyright (C) 2016 D Levin
 * Benchmark source code is dual-licensed under MIT and GPL 2 or later
 * See LICENSE.txt for details
 */
#pragma once

#include "benchmark.hpp"
#include "biquad.h"

#include "kfr/base/univector.hpp"
#include "kfr/dsp/biquad.hpp"
#include "kfr/expressions/pointer.hpp"
#include "kfr/misc/random.hpp"
#include "kfr/version.hpp"
#include <string>

class dsp_benchmark
{
public:
    dsp_benchmark(const char* param) : count(std::atol(param)), out(biquad_size), in(biquad_size)
    {
        cometa::cswitch(
            cometa::csizes<1, 2, 4, 8, 16, 32, 64>, count,
            [this](auto count) {
                kfr::biquad_params<real> bq[count];
                for (size_t i      = 0; i < count; i++)
                    bq[i]          = kfr::biquad_notch<real>((i + 1.0) * 0.5 / (count + 2), 0.5);
                constexpr size_t w = count >= 8 ? kfr::vector_width<real, kfr::cpu_t::native> : 0;
                if (count == 1)
                {
                    this->func = [this, bq]() mutable {
                        kfr::process<real, kfr::cpu_t::native, w>(out, kfr::biquad(bq[0], in), biquad_size);
                    };
                }
                else
                {
                    this->func = [this, bq]() mutable {
                        kfr::process<real, kfr::cpu_t::native, w>(out, kfr::biquad(bq, in), biquad_size);
                    };
                }
            },
            [param]() {
                printf("Invalid param: %s\n", param);
                std::abort();
            });
    }
    static std::string name() { return kfr::library_version(); }
    static std::string shortname() { return "KFR"; }
    void before() { fill_random(0, in.data(), biquad_size); }
    void after() { dont_optimize(out.data()); }
    void execute() { func(); }
    ~dsp_benchmark() {}
private:
    size_t count;
    kfr::univector<real> out;
    kfr::univector<real> in;
    cometa::function<void()> func;
};
