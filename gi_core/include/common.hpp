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
#include <execution>

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

	// https://iq.opengenus.org/detect-if-a-number-is-power-of-2-using-bitwise-operators/
	template<size_t N>
	constexpr bool is_power_of_two_v = N && !(N & (N - 1));

	template<size_t ByteCount, typename ElementType, typename = std::enable_if_t<is_power_of_two_v<ByteCount>>>
	bool is_aligned_to_bytes(const ElementType* ptr)
	{
		return (reinterpret_cast<std::uintptr_t>(ptr) & ByteCount == 0);
	}

	template<size_t ElementCount, typename ElementType, typename = std::enable_if_t<is_power_of_two_v<ElementCount>>>
	bool is_aligned_to(const ElementType* ptr)
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
		/// Class encapsulating view to a range of contiguous elements
		/// </summary>
		/// <typeparam name="ElementType">View element type</typeparam>
		template<typename ElementType>
		class view
		{
		public:
			using value_type = ElementType;
			using reference = value_type&;
			using const_reference = const value_type&;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using size_type = size_t;
		private:
			pointer _begin = nullptr;
			pointer _end = nullptr;
		public:
			constexpr view() noexcept = default;
			constexpr view(pointer b, pointer e) noexcept
				: _begin(b)
				, _end(e)
			{}
		public:
			constexpr size_type size() const noexcept { return static_cast<size_t>(_end - _begin); }
			constexpr size_type length() const noexcept { return size(); }
		public:
			constexpr pointer begin() const noexcept { return _begin; }
			constexpr const_pointer cbegin() const noexcept { return begin(); }
			constexpr pointer end() const noexcept { return _end; }
			constexpr const_pointer cend() const noexcept { return end(); }

			constexpr auto rbegin() const noexcept { return std::reverse_iterator(end()); }
			constexpr auto crbegin() const noexcept { return std::reverse_iterator(cend()); }
			constexpr auto rend() const noexcept { return std::reverse_iterator(begin()); }
			constexpr auto crend() const noexcept { return std::reverse_iterator(cbegin()); }

			constexpr pointer data() const noexcept { return begin(); }
		public:
			constexpr reference at(size_t index) const noexcept { return _begin[index]; }
			constexpr reference operator[](size_t index) const noexcept { return at(index); }
		};

		/// <summary>
		/// Class encapsulating view to a read-only range of contiguous elements
		/// </summary>
		/// <typeparam name="ElementType">View element type</typeparam>
		template<typename ElementType>
		class const_view
		{
		public:
			using value_type = ElementType;
			using reference = value_type&;
			using const_reference = const value_type&;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using size_type = size_t;
		private:
			const_pointer _begin = nullptr;
			const_pointer _end = nullptr;
		public:
			constexpr const_view() noexcept = default;
			constexpr const_view(const_pointer b, const_pointer e) noexcept
				: _begin(b)
				, _end(e)
			{}
			constexpr const_view(pointer b, pointer e) noexcept
				: _begin(b)
				, _end(e)
			{}
			constexpr const_view(const view<ElementType>& v) noexcept
				: _begin(v.cbegin())
				, _end(v.cend())
			{}
		public:
			constexpr size_type size() const noexcept { return static_cast<size_t>(_end - _begin); }
			constexpr size_type length() const noexcept { return size(); }
		public:
			constexpr const_pointer cbegin() const noexcept { return _begin; }
			constexpr const_pointer begin() const noexcept { return cbegin(); }
			constexpr const_pointer cend() const noexcept { return _end; }
			constexpr const_pointer end() const noexcept { return cend(); }

			constexpr auto crbegin() const noexcept { return std::reverse_iterator(cend()); }
			constexpr auto rbegin() const noexcept { return crbegin(); }
			constexpr auto crend() const noexcept { return std::reverse_iterator(cbegin()); }
			constexpr auto rend() const noexcept { return crend(); }

			constexpr const_pointer data() const noexcept { return cbegin(); }
		public:
			constexpr const_reference at(size_t index) const noexcept { return _begin[index]; }
			constexpr const_reference operator[](size_t index) const noexcept { return at(index); }
		};

		template<typename ElementType, size_t Size>
		view<ElementType> make_view(std::array<ElementType, Size>& range)
		{
			return { std::data(range), std::data(range) + std::size(range) };
		}
		template<typename ElementType, size_t Size>
		view<ElementType> make_view(ElementType (&range)[Size])
		{
			return { std::data(range), std::data(range) + std::size(range) };
		}
		template<typename ElementType>
		view<ElementType> make_view(std::vector<ElementType>& range)
		{
			return { std::data(range), std::data(range) + std::size(range) };
		}

		template<typename ElementType, size_t Size>
		const_view<ElementType> make_const_view(const std::array<ElementType, Size>& range)
		{
			return { std::data(range), std::data(range) + std::size(range) };
		}
		template<typename ElementType, size_t Size>
		const_view<ElementType> make_const_view(const ElementType(&range)[Size])
		{
			return { std::data(range), std::data(range) + std::size(range) };
		}
		template<typename ElementType>
		const_view<ElementType> make_const_view(const std::vector<ElementType>& range)
		{
			return { std::data(range), std::data(range) + std::size(range) };
		}

		/// <summary>
		/// <paramref name="Dimensions"/>-dimensional index offset
		/// </summary>
		/// <typeparam name="Dimensions">Dimensionality of the index offset</typeparam>
		template<size_t Dimensions>
		using offsets = std::array<size_t, Dimensions>;

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

		/// <summary>
		/// <paramref name="Dimensions"/>-dimensional size
		/// </summary>
		/// <typeparam name="Dimensions">Dimensionality of the size</typeparam>
		template<size_t Dimensions>
		using sizes = std::array<size_t, Dimensions>;

		template<size_t Dimensions>
		constexpr size_t total_size(const sizes<Dimensions>& size)
		{
			return std::accumulate(std::begin(size), std::end(size), static_cast<size_t>(1), std::multiplies<size_t>());
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
	}
}