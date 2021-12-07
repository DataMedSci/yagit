#include <data/allocation/fast_float_aligned_allocator_impl.hpp>
#include <data/allocation/aligned_allocator.hpp>

#include <simdpp/simd.h>
#include <simdpp/dispatch/get_arch_raw_cpuid.h>
#define SIMDPP_USER_ARCH_INFO ::simdpp::get_arch_raw_cpuid()

namespace yagit::core::data::detail
{
    namespace SIMDPP_ARCH_NAMESPACE
    {
        namespace detail
        {
            template<size_t VectorSize>
            struct alignas(simdpp::float32<VectorSize>) aligned_float32_block
            {
                float block[VectorSize];
            };

            template<size_t VectorSize>
            struct alignas(simdpp::float64<VectorSize>) aligned_float64_block
            {
                double block[VectorSize];
            };

            template<size_t VectorSize>
            constexpr size_t blocks_count(size_t n)
            {
                return (n / VectorSize) + (n % VectorSize > 0 ? 1 : 0);
            }
        }

        // -------- float32 --------

        constexpr size_t float32_VectorSize = SIMDPP_FAST_FLOAT32_SIZE;
        using float_block = detail::aligned_float32_block<float32_VectorSize>;

        float* allocate_float32(size_t n)
        {
            return reinterpret_cast<float*>(new float_block[detail::blocks_count<float32_VectorSize>(n)]);
        }
        allocation_result<float*> allocate_at_least_float32(size_t n)
        {
            auto bcount = detail::blocks_count<float32_VectorSize>(n);
            return { reinterpret_cast<float*>(new float_block[bcount]), bcount * float32_VectorSize };
        }
        void deallocate_float32(float* p, size_t n)
        {
            delete[] p;
        }

        // -------- float64 --------

        constexpr size_t float64_VectorSize = SIMDPP_FAST_FLOAT64_SIZE;
        using double_block = detail::aligned_float64_block<float64_VectorSize>;

        double* allocate_float64(size_t n)
        {
            return reinterpret_cast<double*>(new double_block[detail::blocks_count<float64_VectorSize>(n)]);
        }
        allocation_result<double*> allocate_at_least_float64(size_t n)
        {
            auto bcount = detail::blocks_count<float64_VectorSize>(n);
            return { reinterpret_cast<double*>(new double_block[bcount]), bcount * float64_VectorSize };
        }
        void deallocate_float64(double* p, size_t n)
        {
            delete[] p;
        }
    }

    // -------- float32 --------

    SIMDPP_MAKE_DISPATCHER((float*)(allocate_float32)((size_t)n));
    SIMDPP_MAKE_DISPATCHER((allocation_result<float*>)(allocate_at_least_float32)((size_t)n));
    SIMDPP_MAKE_DISPATCHER((void)(deallocate_float32)((float*)p, (size_t)n));

    // -------- float64 --------

    SIMDPP_MAKE_DISPATCHER((double*)(allocate_float64)((size_t)n));
    SIMDPP_MAKE_DISPATCHER((allocation_result<double*>)(allocate_at_least_float64)((size_t)n));
    SIMDPP_MAKE_DISPATCHER((void)(deallocate_float64)((double*)p, (size_t)n));
}