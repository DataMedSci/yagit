#pragma once

#include <common.hpp>
#include <data/image/iwritable_image_region.hpp>
#include <data/image/iimage.hpp>

namespace yagit::core::data
{
	/// <summary>
	/// Interface extending iimage functionality.
	/// Should allow user to save data.
	/// </summary>
	/// <typeparam name="ElementType">Data point element type</typeparam>
	/// <typeparam name="Dimensions">Dimensionality of image data point storage</typeparam>
	template<typename ElementType, size_t Dimensions>
	class iwritable_image
		: public iwritable_image_region<ElementType, Dimensions>
		, public iimage<ElementType, Dimensions>
	{
	public:
		virtual ~iwritable_image() override = default;
	};
}