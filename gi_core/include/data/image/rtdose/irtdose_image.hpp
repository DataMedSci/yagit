#pragma once

#include <common.hpp>
#include <data/image/rtdose/irtdose_image_region.hpp>
#include <data/image/iimage.hpp>

namespace yagit::core::data
{
	// <summary>
	/// Interface extending iimage functionality.
	/// Represents RTDOSE dicom image.
	/// </summary>
	/// <typeparam name="ElementType">Data point element type</typeparam>
	/// <typeparam name="Dimensions">Dimensionality of image data point storage</typeparam>
	template<typename ElementType, size_t Dimensions>
	class irtdose_image
		: public virtual irtdose_image_region<ElementType, Dimensions>
		, public virtual iimage<ElementType, Dimensions>
	{
	public:
		virtual ~irtdose_image() override = default;
	};

	template<typename ElementType>
	class irtdose_image<ElementType, 0> : public virtual iimage<ElementType, 0>, public virtual irtdose_image_region<ElementType, 0> {};
}