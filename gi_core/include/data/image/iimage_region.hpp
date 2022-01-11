#pragma once

#include <common.hpp>
#include <data/image/image_data.hpp>

namespace yagit::core::data
{
	/// <summary>
	/// Interface representing region of image. Should not contain image data
	/// but rather be a handle to some image resource that can be loaded into memory
	/// on demand
	/// </summary>
	/// <typeparam name="ElementType">Data point element type</typeparam>
	/// <typeparam name="Dimensions">Dimensionality of image data point storage</typeparam>
	template<typename ElementType, size_t Dimensions>
	class iimage_region
	{
	public:
		virtual ~iimage_region() = default;
	public:
		/// <summary>
		/// <para>Region of original image represented by this iimage_region. (relative to iimage - not parent iimage_region) </para>
		/// </summary>
		/// <returns>Region of original image represented by this iimage_region.</returns>
		virtual data_region<Dimensions> region() const noexcept = 0;

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
		unique_ptr<iimage_region<ElementType, Dimensions>> subregion(const data_region<Dimensions>& region, error_code& ec) const
		{
			return unique_ptr<iimage_region<ElementType, Dimensions>>(create_subregion(region, ec));
		}
		unique_ptr<iimage_region<ElementType, Dimensions>> subregion(const data_region<Dimensions>& region) const
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
		unique_ptr<iimage_region<ElementType, Dimensions - 1>> slice(size_t index, error_code& ec)
		{
			return create_slice(Dimension, index, ec);
		}
		template<size_t Dimension, typename = std::enable_if_t<(Dimension > 1)>>
		unique_ptr<iimage_region<ElementType, Dimensions - 1>> slice(size_t index)
		{
			error_code ec;
			return slice<Dimension>(index, ec);
		}

		/// <summary>
		/// <para>Loads iimage_region data into memory buffer (image_data)</para>
		/// <para>TODO: describe and implement reasonable exception-less error handling</para>
		/// </summary>
		/// <typeparam name="Allocator">Allocator type to use for data storage allocation</typeparam>
		/// <param name="format">: Data format in which data points should be loaded</param>
		/// <param name="allocator">: Allocator to use for data storage allocation</param>
		/// <param name="ec">Error code</param>
		/// <returns>Loaded image data into memory buffer (image_data)</returns>
		template<typename Allocator = std::allocator<ElementType>>
		optional<image_data<ElementType, Dimensions>> load(Allocator allocator, error_code& ec) const
		{
			size_t required_size = 0;

			if (ec = load(nullptr, required_size))
				return nullopt;

			auto storage = allocate_at_least_for_image_data(std::move(allocator), required_size);

			if (ec = load(storage.get(), required_size))
				return nullopt;

			return image_data(std::move(storage), region(), required_size);
		}
		template<typename Allocator = std::allocator<ElementType>>
		optional<image_data<ElementType, Dimensions>> load(Allocator allocator) const
		{
			error_code ec;
			return load(allocator, ec);
		}
	protected:
		/// <summary>
		/// <para>Implementable method responsible for data points loading</para>
		/// </summary>
		/// <param name="data_storage">: Pointer to contiguous memory block to which data points should be loaded.</param>
		/// <param name="format">: Format in which data points should be loaded. If data_storage is nullptr, this is argument is ignored.</param>
		/// <param name="required_size">
		/// : returns required size for data points loading imposed by implementation must 
		/// be greater of equal to regular C array of type ElementType of size required to fit all data points in region().
		/// If data_storage is nullptr, this argument returns implementation based required data points storage size.
		/// Else is treated as an input informing about size of allocated storage, handling as input is implementation defined.
		/// </param>
		/// <returns>Error code informing about error that occured during execution unless success</returns>
		virtual error_code load(ElementType* data_storage, size_t& required_size) const = 0;

		/// <summary>
		/// <para>Implementable method responsive for slicing iimage_region</para>
		/// </summary>
		/// <param name="dimension">: dimension along which slice should taken</param>
		/// <param name="index">: index along dimension-axi at which the slice should be taken</param>
		/// <param name="ec">Error code</param>
		/// <returns>Slice of iimage_region unless implementation error occured then nullptr</returns>
		virtual iimage_region<ElementType, Dimensions - 1>* create_slice(size_t dimension, size_t index, error_code& ec) const = 0;

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
		virtual iimage_region<ElementType, Dimensions>* create_subregion(const data_region<Dimensions>& region, error_code& ec) const = 0;
	protected:
		template<typename Allocator>
		static shared_ptr<ElementType[]> allocate_at_least_for_image_data(Allocator allocator, size_t& required_size)
		{
			if constexpr (detail::has_allocate_at_least_method_v<Allocator, ElementType*>)
			{
				auto allocation_result = allocator.allocate_at_least(required_size);
				required_size = allocation_result.count;
				return shared_ptr<ElementType[]>(
					allocation_result.ptr,
					[allocator, allocated_size = required_size](ElementType* ptr) mutable { allocator.deallocate(ptr, allocated_size); },
					allocator
				);
			}
			else
			{
				auto memory = allocator.allocate(required_size);
				return shared_ptr<ElementType[]>(
					memory,
					[allocator, allocated_size = required_size](ElementType* ptr) mutable { allocator.deallocate(ptr, allocated_size); },
					allocator
				);
			}
		}
	};

	template<typename ElementType>
	class iimage_region<ElementType, 0> {};
}