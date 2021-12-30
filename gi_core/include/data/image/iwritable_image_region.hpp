#pragma once

#include <common.hpp>
#include <data/image/iimage_region.hpp>

namespace yagit::core::data
{
	/// <summary>
	/// Interface extending iimage_region functionality.
	/// Should allow user to save data.
	/// </summary>
	/// <typeparam name="ElementType">Data point element type</typeparam>
	/// <typeparam name="Dimensions">Dimensionality of image data point storage</typeparam>
	template<typename ElementType, size_t Dimensions>
	class iwritable_image_region
		: public virtual iimage_region<ElementType, Dimensions>
	{
	public:
		virtual ~iwritable_image_region() = default;
	public:
		/// <summary>
		/// <para>Implementable method responsible for data points saving</para>
		/// </summary>
		/// <param name="data">: Image data points to be saved.</param>
		/// <returns>Error code informing about error that occured during execution unless success</returns>
		virtual error_code save(const image_data<ElementType, Dimensions>& data) = 0;

		/// <summary>
		/// <para>Obtains an iimage_region corresponding to provided subregion.</para>
		/// <para>Provided subregion should be interpreted locally i.e subregion is a subregion of region()
		/// not whole image subregion</para>
		/// <para>Can return null in four cases:</para>
		/// <para> - provided region is bigger (in size) than region() </para>
		/// <para> - provided region's offset is outside region() </para>
		/// <para> - provided region is invalid i.e one of subregion sizes is 0</para>
		/// <para> - implementation doesn't allow for taking subregions of this iimage_region</para>
		/// </summary>
		/// <param name="region">Subregion relative to region()</param>
		/// <param name="ec">Error code</param>
		/// <returns>iimage_region containing subregion defined by region</returns>
		unique_ptr<iwritable_image_region<ElementType, Dimensions>> subregion(const data_region<Dimensions>& region, error_code& ec) const
		{
			return unique_ptr<iwritable_image_region<ElementType, Dimensions>>(create_subregion(region, ec));
		}
		unique_ptr<iwritable_image_region<ElementType, Dimensions>> subregion(const data_region<Dimensions>& region) const
		{
			error_code ec;
			return subregion(region, ec);
		}

		/// <summary>
		/// <para>Returns image slice along <paramref name="Dimension"/> axi at <paramref name="index"/></para>
		/// <para>Can return null in two cases</para>
		/// <para> - index is outside the size of region() at specified axi</para>
		/// <para> - implementation doesn't allow slicing</para>
		/// </summary>
		/// <param name="index">Index along specified dimenion at which slice should be taken</param>
		/// <param name="ec">Error code</param>
		/// <returns>Lower dimensional slice of iimage_region</returns>
		template<size_t Dimension, typename = std::enable_if_t<(Dimension > 1)>>
		unique_ptr<iwritable_image_region<ElementType, Dimensions - 1>> slice(size_t index, error_code& ec)
		{
			return create_slice(Dimension, index, ec);
		}
		template<size_t Dimension, typename = std::enable_if_t<(Dimension > 1)>>
		unique_ptr<iwritable_image_region<ElementType, Dimensions - 1>> slice(size_t index)
		{
			error_code ec;
			return slice<Dimension>(index, ec);
		}
	protected:
		/// <summary>
		/// <para>Obtains an iimage_region corresponding to provided subregion.</para>
		/// <para>Provided subregion should be interpreted locally i.e subregion is a subregion of region()
		/// not whole image subregion</para>
		/// <para>Can return null in four cases:</para>
		/// <para> - provided region is bigger (in size) than region() </para>
		/// <para> - provided region's offset is outside region() </para>
		/// <para> - provided region is invalid i.e one of subregion sizes is 0</para>
		/// <para> - implementation doesn't allow for taking subregions of this iimage_region</para>
		/// </summary>
		/// <param name="region">Subregion relative to region()</param>
		/// <param name="ec">Error code</param>
		/// <returns>transfers ownership of iimage_region containing subregion defined by region</returns>
		virtual iwritable_image_region<ElementType, Dimensions>* create_subregion(const data_region<Dimensions>& region, error_code& ec) const = 0;

		/// <summary>
		/// <para>Implementable method responsive for slicing iimage_region</para>
		/// </summary>
		/// <param name="dimension">: dimension along which slice should taken</param>
		/// <param name="index">: index along dimension-axi at which the slice should be taken</param>
		/// <param name="ec">Error code</param>
		/// <returns>Slice of iimage_region unless implementation error occured then nullptr</returns>
		virtual iwritable_image_region<ElementType, Dimensions - 1>* create_slice(size_t dimension, size_t index, error_code& ec) const = 0;
	};

	template<typename ElementType>
	class iwritable_image_region<ElementType, 0> : public virtual iimage_region<ElementType, 0> {};
}