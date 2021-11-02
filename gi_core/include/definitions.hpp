#pragma once

#include <cstddef>

namespace yagit::core
{
	using size_t = std::size_t;

	namespace data
	{
		template<size_t Dimensions>
		struct offset;
		
		template<size_t Dimensions>
		struct sizes;

		template<size_t Dimensions>
		constexpr size_t total_size(const sizes<Dimensions>& size);

		template<size_t Dimensions>
		struct data_region;

		struct order_element_t;

		template<size_t Dimensions>
		struct data_format;

		template<typename ElementType, size_t Dimensions>
		class iimage;

		template<typename ElementType, size_t Dimensions>
		class iimage_region;

		template<typename ElementType, size_t Dimensions>
		class image_data;

		template<typename ElementType, size_t Dimensions>
		class image_coordinates;
	}
}