#pragma once

#include <common.hpp>

namespace yagit::core::data
{
    /// <summary>
    /// Allocator providing a mean to allocate memory for storing
    /// float32/64 data in such a way that math operations made on
    /// them will not suffer from its memory placement.
    /// If supplied type is not of float32/float64 type it default
    /// to std::allocator
    /// </summary>
    /// <typeparam name="Type">Underlying type</typeparam>
    template<typename Type>
    class fast_float_aligned_allocator
            : std::allocator<Type> // by default falls back to std::allocator
    {
    };

    template<>
    class fast_float_aligned_allocator<float>
    {
    public:
        using value_type = float;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;
    public:
        constexpr fast_float_aligned_allocator() noexcept = default;
        constexpr fast_float_aligned_allocator(const fast_float_aligned_allocator& other) noexcept = default;
        constexpr fast_float_aligned_allocator(fast_float_aligned_allocator&& other) noexcept = default;
    public:
        constexpr fast_float_aligned_allocator& operator=(const fast_float_aligned_allocator& other) noexcept = default;
        constexpr fast_float_aligned_allocator& operator=(fast_float_aligned_allocator&& other) noexcept = default;
    public:
        constexpr ~fast_float_aligned_allocator() = default;
    public:
        value_type* allocate(size_type n);
        allocation_result<value_type*> allocate_at_least(size_type n);
        void deallocate(value_type* p, size_type n);
    public:
        constexpr bool operator==(const fast_float_aligned_allocator& other)
        {
            return true;
        }
        constexpr bool operator!=(const fast_float_aligned_allocator& other)
        {
            return !operator==(other);
        }
    };

    template<>
    class fast_float_aligned_allocator<double>
    {
    public:
        using value_type = double;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;
    public:
        constexpr fast_float_aligned_allocator() noexcept = default;
        constexpr fast_float_aligned_allocator(const fast_float_aligned_allocator& other) noexcept = default;
        constexpr fast_float_aligned_allocator(fast_float_aligned_allocator&& other) noexcept = default;
    public:
        constexpr fast_float_aligned_allocator& operator=(const fast_float_aligned_allocator& other) noexcept = default;
        constexpr fast_float_aligned_allocator& operator=(fast_float_aligned_allocator&& other) noexcept = default;
    public:
        constexpr ~fast_float_aligned_allocator() = default;
    public:
        value_type* allocate(size_type n);
        allocation_result<value_type*> allocate_at_least(size_type n);
        void deallocate(value_type* p, size_type n);
    public:
        constexpr bool operator==(const fast_float_aligned_allocator& other)
        {
            return true;
        }
        constexpr bool operator!=(const fast_float_aligned_allocator& other)
        {
            return !operator==(other);
        }
    };
}