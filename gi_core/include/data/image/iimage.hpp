#pragma once

#include <common.hpp>
#include <data/image/iimage_region.hpp>

namespace yagit::core::data
{
	/// <summary>
	/// Interface representing whole image. Should not contain image data
	/// but rather handle to some image resource that can be loaded into memory
	/// on demand
	/// </summary>
	/// <typeparam name="ElementType">Data point element type</typeparam>
	/// <typeparam name="Dimensions">Dimensionality of image data point storage</typeparam>
	template<typename ElementType, size_t Dimensions>
	class iimage
		: public virtual iimage_region<ElementType, Dimensions>
	{
	public:
		virtual ~iimage() override = default;
	public:
		virtual sizes<Dimensions> size() const = 0;
	};
}