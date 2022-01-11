#pragma once

#include <data/image/image_data.hpp>
#include <data/image/dose/dose_image_coordinates.hpp>

namespace yagit::core::data
{
	/// <summary>
	/// Class encapsulating storage for image data points
	/// </summary>
	/// <typeparam name="ElementType">Data point element type</typeparam>
	/// <typeparam name="Dimensions">Dimensionality of image data point storage</typeparam>
	template<typename ElementType, size_t Dimensions, typename Allocator = std::allocator<ElementType>>
	class dose_image_data
		: image_data<ElementType, Dimensions>
	{
	public:
		using value_type = ElementType;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		constexpr size_t dimensions = Dimensions;
		using coordinates_type = variant<uniform_image_coordinates<ElementType, Dimensions>, nonuniform_image_coordinates<ElementType, Dimensions, Allocator>>;
	private:
		coordinates_type _coordinates;
	public:
		/// <summary>
		/// Constructs image data point storage
		/// </summary>
		/// <param name="data">Pointer to data points array</param>
		/// <param name="image_region">Image region of data points</param>
		/// <param name="data_format">Format of stored data points</param>
		/// <param name="real_total_size">Real size of data point array</param>
		constexpr dose_image_data(
			shared_ptr<ElementType[]>&& data,
			coordinates_type&& coordinates,
			const data_region<Dimensions>& image_region,
			const size_t real_total_size,
		)
			: image_data<ElementType, Dimensions>(data, image_region, real_total_size)
			, _coordinates(std::move(coordinates))
		{
		}
	public:
		coordinates_type& coordinates() noexcept
		{
			return _coordinates;
		}
		const coordinates_type& coordinates() const noexcept
		{
			return _coordinates;
		}
	protected:
		template<size_t Dimension>
		constexpr ElementType position_to_normalized_coords(ElementType position)
		{
			return std::visit([position, this](auto&& coords) {
				return static_cast<ElementType>(coords->position_to_voxel_index<Dimension>(position)) / (region().size[Dimension] - 1)
				}, _coordinates);
		}

		template<size_t... I>
		constexpr void sample_position(
			core::view<ElementType> output_b,
			core::view<ElementType> output_e,
			array<core::const_view<ElementType>, Dimensions> positions,
			index_sequence<I...> _i)const
		{
			while (output_b != output_e)
			{
				*output_b++ = sample_uniform({ position_to_normalized_coords<I>(*positions[I]++)... }, _i);
			}
		}
	public:
		constexpr void sample_position(
			core::view<ElementType> output_b,
			core::view<ElementType> output_e,
			const array<core::const_view<ElementType>, Dimensions>& positions)
		{
			sample_position(output_b, output_e, positions, std::make_index_sequence<Dimensions>());
		}
	};
}