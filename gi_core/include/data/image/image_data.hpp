#pragma once

#include <common.hpp>
#include <memory>

namespace yagit::core::data
{
	/// <summary>
	/// Class encapsulating storage for image data points
	/// </summary>
	/// <typeparam name="ElementType">Data point element type</typeparam>
	/// <typeparam name="Dimensions">Dimensionality of image data point storage</typeparam>
	template<typename ElementType, size_t Dimensions>
	class image_data
	{
	private:
		shared_ptr<ElementType[]> _data;
		const data_region<Dimensions> _image_region;
		const data_format<Dimensions> _data_format;
		const size_t _real_total_size;
	public:
		using value_type = ElementType;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
	public:
		/// <summary>
		/// Constructs image data point storage
		/// </summary>
		/// <param name="data">Pointer to data points array</param>
		/// <param name="image_region">Image region of data points</param>
		/// <param name="data_format">Format of stored data points</param>
		/// <param name="real_total_size">Real size of data point array</param>
		constexpr image_data(
			shared_ptr<ElementType[]>&& data,
			const data_region<Dimensions>& image_region,
			const data_format<Dimensions>& data_format,
			const size_t real_total_size
		)
			: _data(data)
			, _image_region(image_region)
			, _data_format(data_format)
			, _real_total_size(real_total_size)
		{
		}
	public:
		/// <summary></summary>
		/// <returns>Sizes of each dimension of stored data points</returns>
		constexpr sizes<Dimensions> dimension_sizes() const noexcept { return _image_region.size; }
		/// <summary></summary>
		/// <returns>Image region of stored data points</returns>
		constexpr data_region<Dimensions> region() const noexcept { return _image_region; }
		/// <summary></summary>
		/// <returns>Format of stored data points</returns>
		constexpr data_format<Dimensions> format() const noexcept { return _data_format; }
	public:
		/// <summary></summary>
		/// <typeparam name="Dimension">Dimension from which the size is retrieved</typeparam>
		/// <returns>Size of stored data points at specified Dimension</returns>
		template<size_t Dimension, typename = std::enable_if_t<(Dimension < Dimensions)>>
		constexpr size_t size() const noexcept {return _image_region.size[Dimension]; }

		/// <summary></summary>
		/// <returns>Total size of stored data points</returns>
		constexpr size_t total_size() const noexcept { return data::total_size(dimension_sizes()); }
		/// <summary></summary>
		/// <returns>Total allocated memory holding stored data points</returns>
		constexpr size_t real_total_size() const noexcept { return _real_total_size; }

		/// <summary></summary>
		/// <typeparam name="ElementMultiple">Count of elements to which alignment of the pointer to the stored data points should be checked</typeparam>
		/// <returns>Checks if stored data points are aligned to sizeof(ElementType)*ElementMultiple</returns>
		template<size_t ElementMultiple, typename = std::enable_if_t<detail::is_power_of_two_v<ElementMultiple>>>
		constexpr bool is_aligned_to() const noexcept { return core::is_aligned_to<ElementMultiple>(data()); }
	public:
		/// <summary></summary>
		/// <returns>Const pointer to first stored data element</returns>
		constexpr const_pointer data() const noexcept { return _data.get(); }
		/// <summary></summary>
		/// <returns>Mutable pointer to first stored data element</returns>
		constexpr pointer data() noexcept { return _data.get(); }
	};

	template<typename ElementType, size_t Dimensions>
	image_data(shared_ptr<ElementType[]>&&, const data_region<Dimensions>&, const data_format<Dimensions>&, size_t)->image_data<ElementType, Dimensions>;
}