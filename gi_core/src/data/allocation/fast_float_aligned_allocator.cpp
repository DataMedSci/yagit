#include <data/allocation/fast_float_aligned_allocator.hpp>
#include <data/allocation/fast_float_aligned_allocator_impl.hpp>

namespace yagit::core::data
{
    // -------- float32 --------

    float* fast_float_aligned_allocator<float>::allocate(size_type n)
    {
        return detail::allocate_float32(n);
    }

    allocation_result<float*> fast_float_aligned_allocator<float>::allocate_at_least(size_type n)
    {
        return detail::allocate_at_least_float32(n);
    }

    void fast_float_aligned_allocator<float>::deallocate(value_type* p, size_type n)
    {
        detail::deallocate_float32(p, n);
    }

    // -------- float64 --------

    double* fast_float_aligned_allocator<double>::allocate(size_type n)
    {
        return detail::allocate_float64(n);
    }

    allocation_result<double*> fast_float_aligned_allocator<double>::allocate_at_least(size_type n)
    {
        return detail::allocate_at_least_float64(n);
    }

    void fast_float_aligned_allocator<double>::deallocate(value_type* p, size_type n)
    {
        detail::deallocate_float64(p, n);
    }
}