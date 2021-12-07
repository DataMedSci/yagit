#pragma once

#include <common.hpp>
#include <data/image/iimage_region.hpp>
#include <data/image/rtdose/image_coordinates.hpp>

namespace yagit::core::data
{
	// <summary>
	/// Interface extending iimage_region functionality.
	/// Represents region of RTDOSE dicom image.
	/// </summary>
	/// <typeparam name="ElementType">Data point element type</typeparam>
	/// <typeparam name="Dimensions">Dimensionality of image data point storage</typeparam>
	template<typename ElementType, size_t Dimensions>
	class irtdose_image_region
		: public virtual iimage_region<ElementType, Dimensions>
	{
	public:
		virtual ~irtdose_image_region() = default;
	public:
		/// <summary></summary>
		/// <returns>Preferred implementation format to load the data point coordinates</returns>
		template<size_t Dimension>
		data_format<Dimensions> preferred_coordinates_format() const
		{
			return preferred_coordinates_format(Dimension);
		}

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
		unique_ptr<irtdose_image_region<ElementType, Dimensions>> subregion(const data_region<Dimensions>& region, error_code& ec) const
		{
			return unique_ptr<irtdose_image_region<ElementType, Dimensions>>(create_subregion(region, ec));
		}
		unique_ptr<irtdose_image_region<ElementType, Dimensions>> subregion(const data_region<Dimensions>& region) const
		{
			error_code ec;
			return subregion(region, ec);
		}

		// !! assumption about image coordinates type may be wrong as some image can have coordinates of type different
		// to the data point type, this simplification that coordinate_type == data_point_type is strictly made to 
		// unify all possible coordinate_types (to floating point type - ElementType) and simplify calculations that are using
		// those coordinates as additional input along data points !!

		/// <summary>
		/// <para>Loads iimage_region image data point coordinates into memory buffer (image_coordinates)</para>
		/// <para>iimage_region data point coordinats should be in the same order as corresponding data points</para>
		/// <para>obtained by <see cref="iimage_region::load"/> provided they were obtained with the same data_format</para>
		/// <para>TODO: describe and implement reasonable exception-less error handling</para>
		/// </summary>
		/// <typeparam name="Allocator">Allocator type to use for data storage allocation</typeparam>
		/// <param name="format">: Data format in which data point coordinates should be loaded</param>
		/// <param name="allocator">: Allocator to use for data storage allocation</param>
		/// <param name="ec">Error code</param>
		/// <returns>Loaded image data point coordinates into memory buffer (image_data)</returns>
		template<size_t Dimension, typename Allocator = std::allocator<ElementType>, typename = std::enable_if_t<(Dimension < Dimensions)>>
		optional<image_coordinates<ElementType, Dimensions>> load_coordinates(const data_format<Dimensions>& format, Allocator allocator, error_code& ec) const
		{
			size_t required_size = 0;

			if (ec = load_coordinates(nullptr, Dimension, format, required_size))
				return nullopt;

			auto storage = this->allocate_at_least_for_image_data(std::move(allocator), required_size);

			if (ec = load_coordinates(storage.get(), Dimension, format, required_size))
				return nullopt;

			return image_coordinates(std::move(storage), this->region(), format, required_size);
		}
		template<size_t Dimension, typename Allocator = std::allocator<ElementType>, typename = std::enable_if_t<(Dimension < Dimensions)>>
		optional<image_coordinates<ElementType, Dimensions>> load_coordinates(const data_format<Dimensions>& format, Allocator allocator) const
		{
			error_code ec;
			return load_coordinates(format, allocator, ec);
		}
	protected:
		/// <summary></summary>
		/// <returns>Preferred implementation format to load the data point coordinates</returns>
		virtual data_format<Dimensions> preferred_coordinates_format(size_t dimension) const = 0;

		/// <summary>
		/// <para>Implementable method responsible for data point coordinates loading</para>
		/// </summary>
		/// <param name="data_storage">: Pointer to contiguous memory block to which data point coordinates should be loaded.</param>
		/// <param name="format">: Format in which data point coordinates should be loaded. If data_storage is nullptr, this is argument is ignored.</param>
		/// <param name="required_size">
		/// : returns required size for data point coordinates loading imposed by implementation must 
		/// be greater of equal to regular C array of type ElementType of size required to fit all data points in region().
		/// If data_storage is nullptr, this argument returns implementation based required data points storage size.
		/// Else is treated as an input informing about size of allocated storage, handling as input is implementation defined.
		/// </param>
		/// <returns>Error code informing about error that occured during execution unless success</returns>
		virtual error_code load_coordinates(ElementType* data_storage, size_t dimension, const data_format<Dimensions>& format, size_t& required_size) const = 0;

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
		virtual irtdose_image_region<ElementType, Dimensions>* create_subregion(const data_region<Dimensions>& region, error_code& ec) const = 0;
	};
}