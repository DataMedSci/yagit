#pragma once

#include <common.hpp>
#include <data/image_data.hpp>

namespace yagit::core::data
{
	/// <summary>
	/// Class encapsulating storage for image data point coordinated
	/// </summary>
	/// <typeparam name="ElementType">Data point coordinate element type</typeparam>
	/// <typeparam name="Dimensions">Dimensionality of image data point storage</typeparam>
	template<typename ElementType, size_t Dimensions>
	class image_coordinates
		: public image_data<ElementType, Dimensions>
	{
	public:
		using image_data<ElementType, Dimensions>::image_data;
	};

	template<typename ElementType, size_t Dimensions>
	image_coordinates(shared_ptr<ElementType[]>&&, const data_region<Dimensions>&, const data_format<Dimensions>&, size_t)->image_coordinates<ElementType, Dimensions>;
}