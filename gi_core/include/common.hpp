#pragma once

#include <definitions.hpp>

#include <system_error>
#include <memory>
#include <array>
#include <optional>
#include <algorithm>
#include <functional>
#include <variant>
#include <numeric>

#if(__cplusplus <= 202002L)
#define ALLOCATE_AT_LEAST_NOT_DEFINED
#endif

namespace yagit::core
{
	using size_t = std::size_t;
	using std::error_code;
	using std::unique_ptr;
	using std::shared_ptr;
	using std::enable_if_t;
	using std::optional;
	using std::nullopt;
	using std::array;
	using std::true_type;
	using std::false_type;
	using std::declval;
	using std::index_sequence;
	using std::make_index_sequence;
	using std::variant;
	using std::numeric_limits;
#ifndef ALLOCATE_AT_LEAST_NOT_DEFINED
	using std::allocation_result;
#endif

	template<typename T>
	using view = T*;

	template<typename T>
	using const_view = const T*;

	template<size_t N>
	constexpr bool is_power_of_two_v = N && !(N & (N - 1));

	template<size_t ByteCount, typename ElementType, typename = std::enable_if_t<is_power_of_two_v<ByteCount>>>
	bool is_aligned_to_bytes(const_view<ElementType> ptr)
	{
		return (reinterpret_cast<std::uintptr_t>(ptr) & ByteCount == 0);
	}

	template<size_t ElementCount, typename ElementType, typename = std::enable_if_t<is_power_of_two_v<ElementCount>>>
	bool is_aligned_to(const_view<ElementType> ptr)
	{
		return is_aligned_to_bytes<sizeof(ElementType)* ElementCount>(ptr);
	}

	namespace data
	{
#ifdef ALLOCATE_AT_LEAST_NOT_DEFINED
		template<typename Pointer>
		struct allocation_result
		{
			Pointer ptr;
			size_t count;
		};
#endif

		namespace detail
		{
			constexpr bool allocate_at_least_is_standard = __cplusplus > 202002L;

			template<size_t N>
			constexpr bool is_power_of_two_v = N != 0 && !(N & (N - 1));

#ifdef ALLOCATE_AT_LEAST_NOT_DEFINED
			template<typename Type, typename PointerType>
			class has_allocate_at_least_method
			{
			private:
				template<typename OType> static constexpr auto test(OType*)
					-> typename std::is_same<
					decltype(declval<OType>().allocate_at_least(std::declval<size_t>())),
					allocation_result<PointerType>
					>::type;
				template<typename OType> static constexpr false_type test(...);
			public:
				static constexpr bool value = decltype(test<Type>(nullptr))::value;
			};

			template<typename Type, typename PointerType>
			constexpr bool has_allocate_at_least_method_v = has_allocate_at_least_method<Type, PointerType>::value;
#else
			template<typename Type, typename PointerType>
			constexpr bool has_allocate_at_least_method_v = true;
#endif
		}

		/// <summary>
		/// <paramref name="Dimensions"/>-dimensional index offset
		/// </summary>
		/// <typeparam name="Dimensions">Dimensionality of the index offset</typeparam>
		template<size_t Dimensions>
		struct offsets
		{
			size_t offset[Dimensions];
		};

		template<size_t Dimensions>
		constexpr bool operator==(const offsets<Dimensions>& left, const offsets<Dimensions>& right) noexcept
		{
			return std::equal(std::begin(left.offset), std::end(left.offset), std::begin(right.offset), std::end(right.offset), std::equal_to<size_t>());
		}

		template<size_t Dimensions>
		constexpr bool operator!=(const offsets<Dimensions>& left, const offsets<Dimensions>& right) noexcept
		{
			return !(left == right);
		}

		/// <summary>
		/// <paramref name="Dimensions"/>-dimensional size
		/// </summary>
		/// <typeparam name="Dimensions">Dimensionality of the size</typeparam>
		template<size_t Dimensions>
		struct sizes
		{
			size_t sizes[Dimensions];
		};

		template<size_t Dimensions>
		constexpr bool operator==(const sizes<Dimensions>& left, const sizes<Dimensions>& right) noexcept
		{
			return std::equal(std::begin(left.sizes), std::end(left.sizes), std::begin(right.sizes), std::end(right.sizes), std::equal_to<size_t>());
		}

		template<size_t Dimensions>
		constexpr bool operator!=(const sizes<Dimensions>& left, const sizes<Dimensions>& right) noexcept
		{
			return !(left == right);
		}

		template<size_t Dimensions>
		constexpr size_t total_size(const sizes<Dimensions>& size)
		{
			return std::accumulate(std::begin(size.sizes), std::end(size.sizes), static_cast<size_t>(0), std::multiplies<size_t>());
		}

		/// <summary>
		/// <paramref name="Dimensions"/>-dimensional data region.
		/// Used for creating data views
		/// </summary>
		/// <typeparam name="Dimensions">Dimensionality of the data region</typeparam>
		template<size_t Dimensions>
		struct data_region
		{
			offsets<Dimensions> offset;
			sizes<Dimensions> size;
		};

		template<size_t Dimensions>
		constexpr bool operator==(const data_region<Dimensions>& left, const data_region<Dimensions>& right) noexcept
		{
			return left.offset == right.offset && left.size == right.size;
		}

		template<size_t Dimensions>
		constexpr bool operator!=(const data_region<Dimensions>& left, const data_region<Dimensions>& right) noexcept
		{
			return left.offset != right.offset || left.size != right.size;
		}

		struct order_element_t { size_t dimension_index; };

		constexpr order_element_t dim_x { 0 };
		constexpr order_element_t dim_y { 1 };
		constexpr order_element_t dim_z { 2 };
		template<size_t Dimension>
		constexpr order_element_t dim = { Dimension };

		constexpr bool operator==(const order_element_t& left, const order_element_t& right) noexcept
		{
			return left.dimension_index == right.dimension_index;
		}

		constexpr bool operator!=(const order_element_t& left, const order_element_t& right) noexcept
		{
			return left.dimension_index != right.dimension_index;
		}

		/// <summary>
		/// Structure describing stored data format
		/// </summary>
		/// <typeparam name="Dimensions">Dimensionality of the format</typeparam>
		template<size_t Dimensions>
		struct data_format
		{
			const array<order_element_t, Dimensions> order;
		};

		namespace detail
		{
			template<size_t Dimensions, size_t... I>
			constexpr array<order_element_t, Dimensions> make_default_order(index_sequence<I...>)
			{
				return { dim<I>... };
			}
		}

		template<size_t Dimensions>
		constexpr data_format<Dimensions> cpp_multidimensional_array_format{ detail::make_default_order<Dimensions>(make_index_sequence<Dimensions>()) };
		template<size_t Dimensions>
		constexpr data_format<Dimensions> default_format = cpp_multidimensional_array_format<Dimensions>;

		template<size_t Dimensions>
		constexpr bool operator==(const data_format<Dimensions>& left, const data_format<Dimensions>& right) noexcept
		{
            return std::equal(std::begin(left.order), std::end(left.order), std::begin(right.order), std::end(right.order), std::equal_to<order_element_t>());
		}

		template<size_t Dimensions>
		constexpr bool operator!=(const data_format<Dimensions>& left, const data_format<Dimensions>& right) noexcept
		{
			return !(left == right);
		}
	}
}