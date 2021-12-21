#pragma once

#include <common.hpp>

namespace yagit::core::data::detail
{
    // -------- float32 --------

    float* allocate_float32(size_t n);
    allocation_result<float*> allocate_at_least_float32(size_t n);
    void deallocate_float32(float* p, size_t n);

    // -------- float64 --------

    double* allocate_float64(size_t n);
    allocation_result<double*> allocate_at_least_float64(size_t n);
    void deallocate_float64(double* p, size_t n);
}