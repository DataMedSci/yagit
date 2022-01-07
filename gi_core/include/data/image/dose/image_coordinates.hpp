#pragma once

#include <common.hpp>
#include <data/image/image_data.hpp>

namespace yagit::core::data
{

	template<typename ElementType, size_t Dimensions>
	using image_position = array<ElementType, Dimensions>;

	template<typename ElementType, size_t Dimensions>
	using uniform_image_spacing = array<ElementType, Dimensions>;

	template<typename ElementType, size_t Dimensions>
	using nonuniform_image_spacing = array<const_view<ElementType>, Dimensions>;

	template<typename ElementType, size_t Dimensions>
	class image_coordinates
	{
	protected:
		image_position<ElementType, Dimensions> _image_position;
	public:
		constexpr image_coordinates(const image_position<ElementType, Dimensions>& img_position)
			: _image_position(img_position)
		{}
	public:
		virtual ~image_coordinates() noexcept = default;
	public:
		constexpr const image_position<ElementType, Dimensions>& position() const noexcept
		{
			return _image_position;
		}
	};

	/// <summary>
	/// Class encapsulating storage for image data point coordinated
	/// </summary>
	/// <typeparam name="ElementType">Data point coordinate element type</typeparam>
	/// <typeparam name="Dimensions">Dimensionality of image data point storage</typeparam>
	template<typename ElementType, size_t Dimensions>
	class uniform_image_coordinates
		: public image_coordinates<ElementType, Dimensions>
	{
	private:
		uniform_image_spacing<ElementType, Dimensions> _image_spacing;
	public:
		constexpr uniform_image_coordinates(const image_position<ElementType, Dimensions>& img_position, const uniform_image_spacing<ElementType, Dimensions>& img_spacing)
			: image_coordinates<ElementType, Dimensions>(img_position)
			, _image_spacing(img_spacing)
		{}
	public:
		constexpr const uniform_image_spacing<ElementType, Dimensions>& spacing() const noexcept
		{
			return _image_spacing;
		}
		template<size_t Dimension>
		constexpr void voxel_indices_to_positions(const_view<size_t> voxel_indices, const_view<size_t> voxel_indices_end, view<ElementType> positions) const noexcept
		{
			std::transform(voxel_indices, voxel_indices_end, positions, [this](auto&& voxel_index)
				{
					return static_cast<ElementType>(image_coordinates<ElementType, Dimensions>::_image_position[Dimension] + _image_spacing[Dimension] * voxel_index);
				}
			);
		}
		template<size_t Dimension>
		constexpr void positions_to_voxel_indices(const_view<ElementType> positions, const_view<ElementType> positions_end, view<size_t> voxel_indices) const noexcept
		{
			std::transform(positions, positions_end, voxel_indices, [this](auto&& position)
				{
					return static_cast<ElementType>(std::round((position - image_coordinates<ElementType, Dimensions>::_image_position[Dimension]) / _image_spacing[Dimension]));
				}
			);
		}
	};

	template<typename ElementType, size_t Dimensions, typename Allocator = std::allocator<ElementType>>
	class nonuniform_image_coordinates
		: public image_coordinates<ElementType, Dimensions>
	{
	private:
		array<std::vector<ElementType, Allocator>, Dimensions> _image_spacing;
		array<std::vector<ElementType, Allocator>, Dimensions> _image_spacing_partial_sum;
		nonuniform_image_spacing<ElementType, Dimensions> _nu_image_spacing;
	private:
		template<size_t Dimension>
		static std::vector<ElementType, Allocator> initialize_image_spacing(
			const nonuniform_image_spacing<ElementType, Dimensions>& img_spacing,
			const sizes<Dimensions>& img_size,
			const Allocator& alloc)
		{
			auto spacings_size = img_size.sizes[Dimension] - 1;
			std::vector<ElementType, Allocator> _image_spacing(spacings_size, alloc);
			std::copy(img_spacing[Dimension], img_spacing[Dimension] + spacings_size, std::begin(_image_spacing));
			return _image_spacing;
		}
		template<size_t... I>
		static array<std::vector<ElementType, Allocator>, Dimensions> initialize_image_spacing(
			const nonuniform_image_spacing<ElementType, Dimensions>& img_spacing,
			const sizes<Dimensions>& img_size,
			const Allocator& alloc,
			std::index_sequence<I...>)
		{
			return {
				initialize_image_spacing<I>(img_spacing, img_size, alloc)...
			};
		}

		template<size_t Dimension>
		static std::vector<ElementType, Allocator> initialize_image_spacing_partial_sum(
			const nonuniform_image_spacing<ElementType, Dimensions>& img_spacing,
			const sizes<Dimensions>& img_size,
			const Allocator& alloc)
		{
			auto spacings_size = img_size.sizes[Dimension] - 1;
			std::vector<ElementType, Allocator> _image_spacing_partial_sum(img_size.sizes[Dimension], static_cast<ElementType>(0), alloc);
			auto b = std::begin(_image_spacing_partial_sum) + 1;
			std::copy(img_spacing[Dimension], img_spacing[Dimension] + spacings_size, b);
			std::partial_sum(b, std::end(_image_spacing_partial_sum), b, std::plus<ElementType>());
			return _image_spacing_partial_sum;
		}
		template<size_t... I>
		static array<std::vector<ElementType, Allocator>, Dimensions> initialize_image_spacing_partial_sum(
			const nonuniform_image_spacing<ElementType, Dimensions>& img_spacing,
			const sizes<Dimensions>& img_size,
			const Allocator& alloc,
			std::index_sequence<I...>)
		{
			return {
				initialize_image_spacing_partial_sum<I>(img_spacing, img_size, alloc)...
			};
		}

		template<size_t... I>
		static nonuniform_image_spacing<ElementType, Dimensions> initialize_nu_image_spacing(
			const array<std::vector<ElementType, Allocator>, Dimensions>& img_spacing,
			std::index_sequence<I...>)
		{
			return {
				img_spacing[I].data()...
			};
		}
	public:
		nonuniform_image_coordinates(
			const image_position<ElementType, Dimensions>& img_position,
			const nonuniform_image_spacing<ElementType, Dimensions>& img_spacing,
			const sizes<Dimensions>& img_size,
			const Allocator& allocator
			)
			: image_coordinates<ElementType, Dimensions>(img_position)
			, _image_spacing(intitialize_image_spacing(img_spacing, img_size, allocator, make_index_sequence<Dimensions>()))
			, _image_spacing_partial_sum(initialize_image_spacing_partial_sum(img_spacing, img_size, allocator, make_index_sequence<Dimensions>()))
			, _nu_image_spacing(initialize_nu_image_spacing(_image_spacing, make_index_sequence<Dimensions>()))
		{ }
	public:
		const uniform_image_spacing<ElementType, Dimensions>& spacing() const noexcept
		{
			return _image_spacing;
		}
		template<size_t Dimension>
		constexpr void voxel_indices_to_positions(const_view<size_t> voxel_indices, const_view<size_t> voxel_indices_end, view<ElementType> positions) const noexcept
		{
			std::transform(voxel_indices, voxel_indices_end, positions, [this](auto&& voxel_index)
				{
					return static_cast<ElementType>(image_coordinates<ElementType, Dimensions>::_image_position[Dimension] + _image_spacing_partial_sum[Dimension][voxel_index]);
				}
			);
		}
		template<size_t Dimension>
		constexpr void positions_to_voxel_indices(const_view<ElementType> positions, const_view<ElementType> positions_end, view<size_t> voxel_indices) const noexcept
		{
			std::transform(positions, positions_end, voxel_indices, [this](auto&& position)
				{
					return position_to_voxel_index<Dimension>(position);
				}
			);
		}
	private:
		template<size_t Dimension>
		constexpr size_t position_to_voxel_index(ElementType position)
		{
			position -= image_coordinates<ElementType, Dimensions>::_image_position[Dimension];
			auto b = std::begin(_image_spacing_partial_sum);
			auto e = std::end(_image_spacing_partial_sum);
			auto it = std::upper_bound(b, e, position);
			auto g_it = std::min(it, e - 1);
			auto l_it = std::min(it - 1, b);
			if (g_it == l_it)
			{
				return l_it - b;
			}
			else
			{
				auto spacing = *g_it - *l_it;
				if (*g_it - position < position - *l_it)
					return l_it - b;
				else
					return g_it - b;
			}
		}
	};
}