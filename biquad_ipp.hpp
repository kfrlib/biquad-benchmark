/**
 * DSP benchmarking tool (http://kfrlib.com)
 * Copyright (C) 2016 D Levin
 * Benchmark source code is dual-licensed under MIT and GPL 2 or later
 * See LICENSE.txt for details
 */
#pragma once

#include "benchmark.hpp"
#include "biquad.h"

#define __int64 long long
#include "ipp/ipp.h"
#include "ipp/ipps.h"
#include <string>

inline bool init_ipp() { return ippInit() == ippStsOk; }
inline void init() { static bool ok = init_ipp(); }

inline void biquad_notch(real* dest, real frequency, real Q)
{
    const real K    = std::tan(3.1415926535897932384626433832795 * frequency);
    const real K2   = K * K;
    const real norm = 1 / (1 + K / Q + K2);
    dest[3]         = 1.0;
    dest[4]         = 2 * (K2 - 1) * norm;
    dest[5]         = (1 - K / Q + K2) * norm;
    dest[0]         = (1 + K2) * norm;
    dest[1]         = 2 * (K2 - 1) * norm;
    dest[2]         = (1 + K2) * norm;
}

#ifdef TYPE_FLOAT

class dsp_benchmark
{
public:
    dsp_benchmark(const char* param)
        : count(std::atol(param)), out(ippsMalloc_32f(biquad_size)), in(ippsMalloc_32f(biquad_size))
    {
        init();
        int bufsize = 0;
        ippsIIRGetStateSize_BiQuad_32f(count, &bufsize);
        buf  = ippsMalloc_8u(bufsize);
        taps = ippsMalloc_32f(count * 6);
        for (size_t i = 0; i < count; i++)
            biquad_notch(taps + i * 6, (i + 1.0) * 0.5 / (count + 2), 0.5);
        ippsIIRInit_BiQuad_32f(&plan, taps, count, NULL, buf);
    }
    static std::string name()
    {
        init();
        const IppLibraryVersion* ver = ippsGetLibVersion();
        return std::string(ver->Name) + ver->Version;
    }
    void before() { fill_random(0, in, biquad_size); }
    void after() { dont_optimize(out); }
    static std::string shortname() { return "IPP"; }
    void execute() { ippsIIR_32f((const Ipp32f*)this->in, (Ipp32f*)this->out, biquad_size, plan); }
    ~dsp_benchmark()
    {
        ippsFree(buf);
        ippsFree(taps);
    }

private:
    size_t count;
    IppsIIRState_32f* plan;
    Ipp32f* taps;
    real* out;
    real* in;
    Ipp8u* buf;
};

#else

class dsp_benchmark
{
public:
    dsp_benchmark(const char* param)
        : count(std::atol(param)), out(ippsMalloc_64f(biquad_size)), in(ippsMalloc_64f(biquad_size))
    {
        init();
        int bufsize = 0;
        ippsIIRGetStateSize_BiQuad_64f(count, &bufsize);
        buf  = ippsMalloc_8u(bufsize);
        taps = ippsMalloc_64f(count * 6);
        for (size_t i = 0; i < count; i++)
            biquad_notch(taps + i * 6, (i + 1.0) * 0.5 / (count + 2), 0.5);

        ippsIIRInit_BiQuad_64f(&plan, taps, count, NULL, buf);
    }
    static std::string name()
    {
        init();
        const IppLibraryVersion* ver = ippsGetLibVersion();
        return std::string(ver->Name) + ver->Version;
    }
    void before() { fill_random(0, in, biquad_size); }
    void after() { dont_optimize(out); }
    static std::string shortname() { return "IPP"; }
    void execute() { ippsIIR_64f((const Ipp64f*)this->in, (Ipp64f*)this->out, biquad_size, plan); }
    ~dsp_benchmark()
    {
        ippsFree(buf);
        ippsFree(taps);
    }

private:
    size_t count;
    IppsIIRState_64f* plan;
    Ipp64f* taps;
    real* out;
    real* in;
    Ipp8u* buf;
};
#endif
