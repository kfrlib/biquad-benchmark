cmake_minimum_required(VERSION 3.0)

if (${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles" OR ${CMAKE_GENERATOR} STREQUAL "MSYS Makefiles")
    if (CMAKE_BUILD_TYPE_INITIALIZED_TO_DEFAULT)
        set(CMAKE_BUILD_TYPE Release)
    endif ()
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_FLAGS --target=x86_64-w64-windows-gnu CACHE STRING "compile flags" FORCE)
    set(CMAKE_C_FLAGS --target=x86_64-w64-windows-gnu CACHE STRING "compile flags" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS --target=x86_64-w64-windows-gnu)
    set(CMAKE_SHARED_LINKER_FLAGS --target=x86_64-w64-windows-gnu)
    set(CMAKE_STATIC_LINKER_FLAGS --target=x86_64-w64-windows-gnu)
endif ()

project(dsp_benchmark)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )

add_compile_options(-fno-exceptions -fno-rtti -mavx -Wno-deprecated-declarations -std=c++11)

option(ENABLE_KFR     "Enable KFR"     ON)
option(ENABLE_IPP     "Enable IPP"     ON)

link_directories(bin)

if (ENABLE_KFR)
    add_executable(dsp_benchmark_kfr_double dsp_benchmark.cpp benchmark.hpp biquad_kfr.hpp)
    target_compile_definitions(dsp_benchmark_kfr_double PRIVATE DSP_TEST="biquad_kfr.hpp")
    target_compile_options(dsp_benchmark_kfr_double PRIVATE -std=c++1y)
    add_executable(dsp_benchmark_kfr_float dsp_benchmark.cpp benchmark.hpp biquad_kfr.hpp)
    target_compile_definitions(dsp_benchmark_kfr_float PRIVATE DSP_TEST="biquad_kfr.hpp" TYPE_FLOAT=1)
    target_compile_options(dsp_benchmark_kfr_float PRIVATE -std=c++1y)
endif ()

if (ENABLE_IPP)
    add_executable(dsp_benchmark_ipp_double dsp_benchmark.cpp benchmark.hpp biquad_ipp.hpp)
    target_compile_definitions(dsp_benchmark_ipp_double PRIVATE DSP_TEST="biquad_ipp.hpp")
    add_executable(dsp_benchmark_ipp_float dsp_benchmark.cpp benchmark.hpp biquad_ipp.hpp)
    target_compile_definitions(dsp_benchmark_ipp_float PRIVATE DSP_TEST="biquad_ipp.hpp" TYPE_FLOAT=1)

    target_link_libraries(dsp_benchmark_ipp_double ippsmt ippcoremt ippvmmt)
    target_link_libraries(dsp_benchmark_ipp_float ippsmt ippcoremt ippvmmt)
endif ()

