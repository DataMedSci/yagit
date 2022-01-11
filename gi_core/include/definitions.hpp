#pragma once

#include <cstddef>
#include <cstdint>

namespace yagit::core
{
	using size_t = std::size_t;
	using ptrdiff_t = std::ptrdiff_t;

	namespace data
	{
		template<size_t Dimensions>
		struct data_region;

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