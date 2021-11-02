#pragma once

#include <definitions.hpp>

#include <system_error>
#include <memory>
#include <optional>

#if(__cplusplus <= 202002L)
#define ALLOCATE_AT_LEAST_NOT_DEFINED
#endif

namespace yagit::core
{
	using size_t = std::size_t;
	using std::error_code;
	using std::shared_ptr;
	using std::enable_if_t;
	using std::optional;
	using std::nullopt;
	using std::true_type;
	using std::false_type;
	using std::declval;
#ifndef ALLOCATE_AT_LEAST_NOT_DEFINED
	using std::allocation_result;
#endif

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
		struct offset
		{
			size_t offsets[Dimensions];
		};

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
		constexpr size_t total_size(const sizes<Dimensions>& size)
		{
			return std::accumulate(dimension_sizes.begin(), dimension_sizes.end(), static_cast<size_t>(0), std::multiplies<size_t>());
		}

		/// <summary>
		/// <paramref name="Dimensions"/>-dimensional data region.
		/// Used for creating data views
		/// </summary>
		/// <typeparam name="Dimensions">Dimensionality of the data region</typeparam>
		template<size_t Dimensions>
		struct data_region
		{
			offset<Dimensions> offset;
			sizes<Dimensions> size;
		};

		struct order_element_t { size_t dimension; };

		constexpr order_element_t dim_x { 0 };
		constexpr order_element_t dim_y { 1 };
		constexpr order_element_t dim_z { 2 };
		template<size_t Dimension>
		constexpr order_element_t dim = { Dimension };

		/// <summary>
		/// Structure describing stored data format
		/// </summary>
		/// <typeparam name="Dimensions">Dimensionality of the format</typeparam>
		template<size_t Dimensions>
		struct data_format
		{
			const std::array<order_element_t, Dimensions> order;
		};

		template<size_t Dimensions>
		constexpr data_format<Dimensions> cpp_multidimensional_array_format{ dim_x, dim_y, dim_z };
		template<size_t Dimensions>
		constexpr data_format<Dimensions> default_format = cpp_multidimensional_array_format;
	}
}