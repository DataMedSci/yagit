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
		: public virtual iwritable_image_region<ElementType, Dimensions>
		, public virtual iimage<ElementType, Dimensions>
	{
	public:
		virtual ~iwritable_image() override = default;
	};

	template<typename ElementType>
	class iwritable_image<ElementType, 0> : public virtual iimage<ElementType, 0>, public virtual iwritable_image_region<ElementType, 0> {};
}