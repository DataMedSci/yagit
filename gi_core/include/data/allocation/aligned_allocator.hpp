#pragma once

#include <common.hpp>

namespace yagit::core::data
{
	namespace detail
	{
		template<typename Type, size_t ElementMultiple, typename = enable_if_t<detail::is_power_of_two_v<ElementMultiple> && (alignof(Type)* ElementMultiple <= alignof(max_align_t))>>
		struct alignas(alignof(Type)* ElementMultiple) aligned_block
		{
			Type block[ElementMultiple];
		};
	}

    /// <summary>
    /// Allocator providing a mean to allocate aligned memory to a boundary
    /// defined by alignment requirement of the underyling type multiplied by
    /// desired multiple of the element alignment requirement (must be a power of 2)
    /// </summary>
    /// <typeparam name="Type">Underlying type</typeparam>
    /// <typeparam name="ElementMultiple">Multiplicity of element alignment requirement (must be a power of 2)</typeparam>
	template<typename Type, size_t ElementMultiple, typename = enable_if_t<detail::is_power_of_two_v<ElementMultiple> && (alignof(Type)* ElementMultiple <= alignof(max_align_t))>>
	class aligned_allocator 
		: protected std::allocator<detail::aligned_block<Type, ElementMultiple>>
	{
	private:
		using base = std::allocator<detail::aligned_block<Type, ElementMultiple>>;
	public:
		using value_type = Type;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;
		using propagate_on_container_move_assignment = typename base::propagate_on_container_move_assignment;
	public:
		constexpr aligned_allocator() noexcept = default;
		constexpr aligned_allocator(const aligned_allocator& other) noexcept = default;
		constexpr aligned_allocator(aligned_allocator&& other) noexcept = default;
	public:
		constexpr aligned_allocator& operator=(const aligned_allocator& other) noexcept = default;
		constexpr aligned_allocator& operator=(aligned_allocator&& other) noexcept = default;
	public:
		constexpr ~aligned_allocator() = default;
	private:
		constexpr size_type blocks_count(size_type n)
		{
			return (n / ElementMultiple) + (n % ElementMultiple > 0 ? 1 : 0);
		}
	public:
		value_type* allocate(size_type n)
		{
			return reinterpret_cast<value_type*>(base::allocate(blocks_count(n)));
		}
		allocation_result<value_type*> allocate_at_least(size_type n)
		{
			auto bcount = blocks_count(n);
			return { reinterpret_cast<value_type*>(base::allocate(bcount)), bcount * ElementMultiple };
		}
		void deallocate(value_type* p, size_type n)
		{
			base::deallocate(p, n);
		}
	public:
		constexpr bool operator==(const aligned_allocator& other)
		{
			return base::operator==(other);
		}
		constexpr bool operator!=(const aligned_allocator& other)
		{
#if(__cplusplus < 202002L)
			return base::operator!=(other);
#else
			return !operator==(other);
#endif
		}
	};
}