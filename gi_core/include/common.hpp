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

	// https://iq.opengenus.org/detect-if-a-number-is-power-of-2-using-bitwise-operators/
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

        namespace detail
        {
            template<size_t Dimensions, size_t... I>
            constexpr offsets<Dimensions> make_zero_offset(index_sequence<I...>)
            {
                return { (I, 0)... };
            }
        }

        template<size_t Dimensions>
        constexpr offsets<Dimensions> zero_offset = detail::make_zero_offset<Dimensions>(std::make_index_sequence<Dimensions>());

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

        template<size_t Dimensions>
        constexpr auto make_temporary_index_flattener(const array<order_element_t, Dimensions>& order, const sizes<Dimensions>& size)
        {
            return [&order, &size](const array<size_t, Dimensions>& index, const offsets<Dimensions>& offsets)
            {
				// e.g 3D index with order = (0,1,2) (default):
				// flat_index = ((0*sizes[0] + index[0])*sizes[1] + index[1])*sizes[2] + index[2]
				// e.g 3D index with order = (2,0,1):
				// flat_index = ((0*sizes[2] + index[2])*sizes[0] + index[0])*sizes[1] + index[1]
				// e.g 3D index with order = (2,0,1) + arbitrary offset:
				// flat_index = ((0*sizes[2] + index[2] + offset[2])*sizes[0] + index[0] + offsets[0])*sizes[1] + index[1] + offset[1]
                auto flat_index = static_cast<size_t>(0);
                for(auto ordi = order.rbegin(); ordi != order.rend(); ++ordi)
                {
					flat_index *= size.sizes[ordi->dimension_index];
					flat_index += index[ordi->dimension_index] + offsets.offset[ordi->dimension_index];
                }
				return flat_index;
            };
        }

		// differs from previous declaration by binding values of order and size in returned lambda
		template<size_t Dimensions>
		constexpr auto make_permament_index_flattener(const array<order_element_t, Dimensions>& order, const sizes<Dimensions>& size)
		{
			return [order, size](const array<size_t, Dimensions>& index, const offsets<Dimensions>& offsets)
			{
				// e.g 3D index with order = (0,1,2) (default):
				// flat_index = ((0*sizes[0] + index[0])*sizes[1] + index[1])*sizes[2] + index[2]
				// e.g 3D index with order = (2,0,1):
				// flat_index = ((0*sizes[2] + index[2])*sizes[0] + index[0])*sizes[1] + index[1]
				// e.g 3D index with order = (2,0,1) + arbitrary offset:
				// flat_index = ((0*sizes[2] + index[2] + offset[2])*sizes[0] + index[0] + offsets[0])*sizes[1] + index[1] + offset[1]
				auto flat_index = static_cast<size_t>(0);
				for (auto ordi = order.rbegin(); ordi != order.rend(); ++ordi)
				{
					flat_index *= size.sizes[ordi->dimension_index];
					flat_index += index[ordi->dimension_index] + offsets.offset[ordi->dimension_index];
				}
				return flat_index;
			};
		}

		namespace detail
		{
			template<typename ElementType, size_t Dimensions, typename F, size_t DI>
			constexpr inline void copy_same_format_roll(
				core::view<ElementType> destination, const data_region<Dimensions>& d_region, const sizes<Dimensions>& d_size,
				core::const_view<ElementType> source, const data_region<Dimensions>& s_region, const sizes<Dimensions>& s_size,
				const data_format<Dimensions>& format,
				const F& index_flattener,
				std::array<size_t, Dimensions>& s_index, std::array<size_t, Dimensions>& d_index
				)
			{
				// last dimension can be copied with memcpy as it is contiguous for both source and destination
				s_index[DI] = 0;
				d_index[DI] = 0;
				auto source_b = source + index_flattener(s_index, s_region.offset);
				auto dest_b = destination + index_flattener(d_index, d_region.offset);

				auto dim_i = format.order[DI].dimension_index;
				std::copy(source_b, source_b + s_region.size.sizes[dim_i], dest_b);
			}

			template<typename ElementType, size_t Dimensions, typename F, size_t DI, size_t DIO, size_t... DIs>
			constexpr inline void copy_same_format_roll(
				core::view<ElementType> destination, const data_region<Dimensions>& d_region, const sizes<Dimensions>& d_size,
				core::const_view<ElementType> source, const data_region<Dimensions>& s_region, const sizes<Dimensions>& s_size,
				const data_format<Dimensions>& format,
				const F& index_flattener,
				std::array<size_t, Dimensions>& s_index, std::array<size_t, Dimensions>& d_index)
			{
                for (s_index[DI] = 0, d_index[DI] = 0;
					s_index[DI] != s_region.size.sizes[DI];
					++s_index[DI], ++d_index[DI])
				{
					copy_same_format_roll<ElementType, Dimensions, F, DIO, DIs...>(
						destination, d_region, d_size, source,s_region, s_size,
						format,
						index_flattener,
						s_index, d_index);
				}
			}

			template<typename ElementType, size_t Dimensions, typename F, size_t... DI>
			constexpr void copy_same_format(
				core::view<ElementType> destination, const data_region<Dimensions>& d_region, const sizes<Dimensions>& d_size,
				core::const_view<ElementType> source, const data_region<Dimensions>& s_region, const sizes<Dimensions>& s_size,
				const data_format<Dimensions>& format,
				const F& index_flattener,
				std::index_sequence<DI...>)
			{
				std::array<size_t, Dimensions> s_index;
				std::array<size_t, Dimensions> d_index;
				copy_same_format_roll<ElementType, Dimensions, F, DI...>(
					destination, d_region, d_size, source, s_region, s_size,
					format,
					index_flattener,
					s_index, d_index);
			}

			template<typename ElementType, size_t Dimensions, typename F0, typename F1, size_t DI>
			constexpr inline void copy_different_format_roll(
				core::view<ElementType> destination, const data_region<Dimensions>& d_region, const sizes<Dimensions>& d_size,
				core::const_view<ElementType> source, const data_region<Dimensions>& s_region, const sizes<Dimensions>& s_size,
				const F0& d_index_flattener,
				const F1& s_index_flattener,
				std::array<size_t, Dimensions>& s_index, std::array<size_t, Dimensions>& d_index
			)
			{
				for (s_index[DI] = 0, d_index[DI] = 0;
					s_index[DI] != s_region.size.sizes[DI];
					++s_index[DI], ++d_index[DI])
				{
					destination[d_index_flattener(d_index, d_region.offset)] = source[s_index_flattener(s_index, s_region.offset)];
				}
			}

			template<typename ElementType, size_t Dimensions, typename F0, typename F1, size_t DI, size_t DIO, size_t... DIs>
			constexpr inline void copy_different_format_roll(
				core::view<ElementType> destination, const data_region<Dimensions>& d_region, const sizes<Dimensions>& d_size,
				core::const_view<ElementType> source, const data_region<Dimensions>& s_region, const sizes<Dimensions>& s_size,
				const F0& d_index_flattener,
				const F1& s_index_flattener,
				std::array<size_t, Dimensions>& s_index, std::array<size_t, Dimensions>& d_index)
			{
				for (s_index[DI] = 0, d_index[DI] = 0;
					s_index[DI] != s_region.size.sizes[DI];
					++s_index[DI], ++d_index[DI])
				{
					copy_different_format_roll<ElementType, Dimensions, F0, F1, DIO, DIs...>(
						destination, d_region, d_size, source, s_region, s_size,
						d_index_flattener,
						s_index_flattener,
						s_index, d_index);
				}
			}

			template<typename ElementType, size_t Dimensions, typename F0, typename F1, size_t... DI>
			constexpr void copy_different_format(
				core::view<ElementType> destination, const data_region<Dimensions>& d_region, const sizes<Dimensions>& d_size,
				core::const_view<ElementType> source, const data_region<Dimensions>& s_region, const sizes<Dimensions>& s_size,
				const F0& d_index_flattener,
				const F1& s_index_flattener,
				std::index_sequence<DI...>)
			{
				std::array<size_t, Dimensions> s_index;
				std::array<size_t, Dimensions> d_index;
				copy_different_format_roll<ElementType, Dimensions, F0, F1, DI...>(
					destination, d_region, d_size, source, s_region, s_size,
					d_index_flattener,
					s_index_flattener,
					s_index, d_index);
			}
		}

        template<typename ElementType, size_t Dimensions>
        constexpr bool copy(
                core::view<ElementType> destination, const data_region<Dimensions>& d_region, const sizes<Dimensions>& d_size, const data_format<Dimensions>& d_format,
                core::const_view<ElementType> source, const data_region<Dimensions>& s_region, const sizes<Dimensions>& s_size, const data_format<Dimensions>& s_format
                )
        {
            if(d_region.size != s_region.size)
                return false;

            if constexpr(Dimensions == 1)
            {
				// 1 dimensional array is a degenerate problem as it has only 1 format and any view of it is by design contiguous in memory
                std::copy(source + s_region.offset.offset[0], source + s_region.offset.offset[0] + s_region.size.sizes[0], destination + d_region.offset.offset[0]);
            }
            else
            {
                if(d_format == s_format)
                {
					// if offsets are zero and region size spans whole array then it is a simple case of copying
                    if(d_region.offset == zero_offset<Dimensions> && s_region.offset == zero_offset<Dimensions> &&
						d_region.size == d_size && s_region.size == s_size)
                    {
						std::copy(source, source + total_size(s_size), destination);
                    }
                    else
                    {
						auto s_if = make_temporary_index_flattener(s_format.order, s_size);
						detail::copy_same_format(destination, d_region, d_size, source, s_region, s_size, s_format, s_if, std::make_index_sequence<Dimensions>());
                    }
                }
                else
                {
					auto s_if = make_temporary_index_flattener(s_format.order, s_size);
					auto d_if = make_temporary_index_flattener(d_format.order, d_size);
					detail::copy_different_format(destination, d_region, d_size, source, s_region, s_size, d_if, s_if, std::make_index_sequence<Dimensions>());
                }
            }

            return true;
        }
	}
}