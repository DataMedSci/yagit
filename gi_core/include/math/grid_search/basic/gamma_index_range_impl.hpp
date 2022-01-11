#pragma once

#include <math/single/basic/gamma_index_single.hpp>
#include <data/image/dose/dose_image_coordinates.hpp>

#include <math/classic/basic/gamma_index_range_impl.hpp>

namespace yagit::core::math::grid_search::basic::detail
{
	using single::basic::gamma_index;

	template<typename ElementType, size_t Dimensions>
	constexpr array<ElementType, Dimensions> operator+(const array<ElementType, Dimensions>& a, const array<ElementType, Dimensions>& b)
	{
		array<ElementType, Dimensions> result;
		std::transform(std::begin(a), std::end(a), std::begin(b), std::begin(result), std::plus<ElementType>());
		return result;
	}

	template<typename ElementType, size_t Dimensions>
	constexpr array<ElementType, Dimensions> operator-(const array<ElementType, Dimensions>& a, const array<ElementType, Dimensions>& b)
	{
		array<ElementType, Dimensions> result;
		std::transform(std::begin(a), std::end(a), std::begin(b), std::begin(result), std::minus<ElementType>());
		return result;
	}

	template<typename ElementType, size_t Dimensions>
	constexpr array<ElementType, Dimensions> operator*(const array<ElementType, Dimensions>& a, const array<ElementType, Dimensions>& b)
	{
		array<ElementType, Dimensions> result;
		std::transform(std::begin(a), std::end(a), std::begin(b), std::begin(result), std::multiplies<ElementType>());
		return result;
	}

	template<typename ElementType, size_t Dimensions>
	constexpr array<ElementType, Dimensions> operator/(const array<ElementType, Dimensions>& a, const array<ElementType, Dimensions>& b)
	{
		array<ElementType, Dimensions> result;
		std::transform(std::begin(a), std::end(a), std::begin(b), std::begin(result), std::divides<ElementType>());
		return result;
	}

	template<typename ElementType, size_t Dimensions>
	constexpr array<ElementType, Dimensions> min(const array<ElementType, Dimensions>& a, const array<ElementType, Dimensions>& b)
	{
		array<ElementType, Dimensions> result;
		std::transform(std::begin(a), std::end(a), std::begin(b), std::begin(result), std::min<ElementType>());
		return result;
	}

	template<typename ElementType, size_t Dimensions>
	constexpr array<ElementType, Dimensions> max(const array<ElementType, Dimensions>& a, const array<ElementType, Dimensions>& b)
	{
		array<ElementType, Dimensions> result;
		std::transform(std::begin(a), std::end(a), std::begin(b), std::begin(result), std::max<ElementType>());
		return result;
	}

	template<typename TargetType, typename ElementType, size_t Dimensions>
	constexpr array<TargetType, Dimensions> convert_to(const array<ElementType, Dimensions>& vs)
	{
		array<TargetType, Dimensions> result;
		std::transform(std::begin(vs), std::end(vs), std::begin(result), [](auto&& v) {return static_cast<TargetType>(v); });
		return result;
	}

	template<size_t Dimensions, typename ElementType>
	constexpr array<ElementType, Dimensions> splat(const ElementType& v)
	{
		array<ElementType, Dimensions> result;
		std::fill(std::begin(result), std::end(result), v);
		return result;
	}

	template<typename ElementType, size_t Dimensions>
	constexpr array<ElementType, Dimensions> ceil(const array<ElementType, Dimensions>& vs)
	{
		array<ElementType, Dimensions> result;
		std::transform(std::begin(vs), std::end(vs), std::begin(result), [](auto&& v) {return std::ceil(v); });
		return result;
	}

	template<size_t Dimensions, typename F, size_t I0, size_t... I>
	constexpr bool iterate_multidim_inclusive_impl(
		const array<size_t, Dimensions>& b, const array<size_t, Dimensions>& e,
		array<long, Dimensions>& index, F f,
		std::index_sequence<I0, I...>)
	{
		if constexpr (sizeof...(I) == 0)
		{
			for (index[I0] = -b[I0]; index[I0] <= e[I0]; ++index[I0])
				if(f(index)) break;
		}
		else
		{
			for (index[I0] = -b[I0]; index[I0] <= e[I0]; ++index[I0])
				if(iterate_multidim_inclusive_impl(b, e, index, f, std::index_sequence<I...>{})) break;
		}
	}

	template<size_t Dimensions, typename F>
	constexpr void iterate_multidim_inclusive(const array<size_t, Dimensions>& b, const array<size_t, Dimensions>& e, F f)
	{
		array<long, Dimensions> index;
		iterate_multidim_inclusive_impl(b, e, index, f, std::make_index_sequence<Dimensions>());
	}

	template<typename ElementType, size_t Dimensions>
	constexpr size_t flatten(const array<size_t, Dimensions>& index, const array<ElementType, Dimensions>& image_size)
	{
		size_t flattened_index = 0;
		for (size_t i = 0; i < Dimensions; i++)
		{
			flattened_index *= image_size[i];
			flattened_index += index[i];
		}
		return flattened_index;
	}

	template<typename ElementType, size_t Dimensions>
	constexpr array<ElementType, Dimensions> zero = splat<Dimensions>(static_cast<ElementType>(0));
	template<typename ElementType, size_t Dimensions>
	constexpr array<ElementType, Dimensions> one = splat<Dimensions>(static_cast<ElementType>(1));

	template<typename ElementType, size_t Dimensions, size_t I0, size_t... I>
	constexpr ElementType sample_nonuniform_impl(
		const_view<ElementType> target_doses,
		const array<ElementType, Dimensions>& pixel_position,
		array<size_t, Dimensions>& pixel_index,
		const array<size_t, Dimensions>& image_size,
		std::index_sequence<I0, I...>
	)
	{
		auto low_pixel_pos = std::clamp<ElementType>(floor(pixel_position[I0]), 0, image_size[I0] - 1);
		auto high_pixel_pos = std::clamp<ElementType>(floor(pixel_position[I0]), 0, image_size[I0] - 1);
		auto low_pixel_index = static_cast<size_t>(low_pixel_pos);
		auto high_pixel_index = static_cast<size_t>(high_pixel_pos);

		if constexpr (sizeof...(I) == 0)
		{
			pixel_index[I0] = low_pixel_index;
			auto low_v = target_doses[flatten(pixel_index)];

			if (low_pixel_index == high_pixel_index)
				return low_v;

			pixel_index[I0] = high_pixel_index;
			auto high_v = target_doses[flatten(pixel_index)];

			auto t = (pixel_position[I0] - low_pixel_pos); // already in 0..1 range
			return std::lerp(low_v, high_v, t);
		}
		else
		{
			pixel_index[I0] = low_pixel_index;
			auto low_v = sample_nonuniform_impl(target_doses, pixel_position, pixel_index, image_size, std::index_sequence<I...>{});

			if (low_pixel_index == high_pixel_index)
				return low_v;

			pixel_index[I0] = high_pixel_index;
			auto high_v = sample_nonuniform_impl(target_doses, pixel_position, pixel_index, image_size, std::index_sequence<I...>{});

			auto t = (pixel_position[I0] - low_pixel_pos); // already in 0..1 range
			return std::lerp(low_v, high_v, t);
		}
	}

	template<typename ElementType, size_t Dimensions>
	constexpr ElementType sample_nonuniform(
		const_view<ElementType> target_doses,
		const array<ElementType, Dimensions>& pixel_position,
		const data::sizes<Dimensions>& image_size
	)
	{
		array<size_t, Dimensions> pixel_index;
		return sample_nonuniform_impl(
			target_doses, pixel_position,
			pixel_index,
			image_size,
			std::make_index_sequence<Dimensions>());
	}

	template<typename ElementType, size_t Dimensions, typename ParamsType, size_t... I>
	constexpr ElementType gamma_index_minimize_pass_single_impl(
		ElementType reference_dose,
		const array<size_t, Dimensions>& reference_pixel_index,
		const data::sizes<Dimensions>& reference_image_size,
		const array<ElementType, Dimensions>& reference_image_position,
		const array<ElementType, Dimensions>& reference_image_spacing,
		const_view<ElementType> target_doses,
		const data::sizes<Dimensions>& target_image_size,
		const array<ElementType, Dimensions>& target_image_position,
		const array<ElementType, Dimensions>& target_image_spacing,
		const array<ElementType, Dimensions>& target_image_position_end,
		const algorithm_version::grid_search::parameters<ElementType, Dimensions>& grid_search_params,
		const ParamsType& params,
		index_sequence<I...>)
	{
		if (std::isnan(reference_dose))
			return reference_dose;

		auto reference_position = reference_image_position + reference_image_spacing * convert_to<ElementType>(reference_pixel_index);
		// map reference pixel position to fractional pixel index on target image
		auto target_initial_pixel_position = (reference_position - target_image_position) / target_image_spacing;
		// calculate maximum amount of steps in negative and positive direction based of step and max_distance
		auto positive_steps = convert_to<int64_t>(ceil(splat<Dimensions>(grid_search_params.max_distance) / grid_search_params.step));
		auto negative_steps = positive_steps;

		auto steps_to_positive_edge = (target_image_position_end - reference_position) / grid_search_params.step;
		auto steps_to_negative_edge = (reference_position - target_image_position) / grid_search_params.step;
		// clamp steps count to image size to limit search space for voxels on edges of target image
		positive_steps = convert_to<int64_t>(ceil(min(steps_to_positive_edge, convert_to<ElementType>(positive_steps))));
		negative_steps = convert_to<int64_t>(ceil(min(steps_to_negative_edge, convert_to<ElementType>(negative_steps))));

		// sample target image with initial pixel position and calculate initial gamma index value
		ElementType min_gi = gamma_index(
			reference_dose,
			sample_nonuniform(target_doses, target_initial_pixel_position, target_image_size),
			reference_position[I]...,
			reference_position[I]...,
			params
		);
		if (min_gi <= static_cast<ElementType>(1.0))
			return min_gi;

		// iterate over each dimension doing negative_steps + positive_steps along them
		// move target_position by amount specified in step parameter
		// and minimize gamma index value
		iterate_multidim_inclusive(negative_steps, positive_steps, [&](auto&& index)
			{
				auto target_position = reference_position + convert_to<ElementType>(index) * grid_search_params.step;
				auto target_pixel_position = (target_position - target_image_position) / target_image_spacing;
				min_gi = std::min(
					min_gi,
					gamma_index(
						reference_dose,
						sample_nonuniform(target_doses, target_pixel_position, target_image_size),
						reference_position[I]...,
						target_position[I]...,
						params
					);
				);
				// stop calculation if minimized gamma index value is below 1.0
				return min_gi <= static_cast<ElementType>(1.0);
			});

		return min_gi;
	}

	template<typename ElementType, size_t Dimensions, typename ParamsType, size_t... I>
	constexpr error_code gamma_index_minimize_pass_impl(
		view<ElementType> gamma_index_output, view<ElementType> gamma_index_output_end,
		const_view<ElementType> reference_doses,
		const array<size_t, Dimensions>& reference_pixel_index,
		const data::sizes<Dimensions>& reference_image_size,
		const array<ElementType, Dimensions>& reference_image_position,
		const array<ElementType, Dimensions>& reference_image_spacing,
		const_view<ElementType> target_doses,
		const data::sizes<Dimensions>& target_image_size,
		const array<ElementType, Dimensions>& target_image_position,
		const array<ElementType, Dimensions>& target_image_spacing,
		const array<ElementType, Dimensions>& target_image_position_end,
		const algorithm_version::grid_search::parameters<ElementType, Dimensions>& grid_search_params,
		const ParamsType& params,
		index_sequence<I...> _i)
	{
		while (gamma_index_output != gamma_index_output_end)
		{
			*gamma_index_output++ = gamma_index_minimize_pass_single_impl(
				*reference_doses++,
				reference_pixel_index,
				reference_image_size,
				reference_image_position,
				reference_image_spacing,
				target_doses,
				target_image_size,
				target_image_position,
				target_image_spacing,
				target_image_position_end,
				grid_search_params,
				params,
				_i
			);
		}

		return {};
	}


	using classic::basic::detail::gamma_index_initialize_pass_impl;
	using classic::basic::detail::gamma_index_finalize_pass_impl;

	template<typename ElementType>
	constexpr error_code gamma_index_initialize_pass(view<ElementType> gamma_index_output, view<ElementType> gamma_index_output_end)
	{
		return gamma_index_initialize_pass_impl(std::execution::seq, gamma_index_output, gamma_index_output_end);
	}

	template<typename ElementType, size_t Dimensions, typename ParamsType>
	constexpr error_code gamma_index_minimize_pass(
		view<ElementType> gamma_index_output, view<ElementType> gamma_index_output_end,
		const_view<ElementType> reference_doses,
		const array<size_t, Dimensions>& reference_pixel_index,
		const data::sizes<Dimensions>& reference_image_size,
		const array<ElementType, Dimensions>& reference_image_position,
		const array<ElementType, Dimensions>& reference_image_spacing,
		const_view<ElementType> target_doses,
		const data::sizes<Dimensions>& target_image_size,
		const array<ElementType, Dimensions>& target_image_position,
		const array<ElementType, Dimensions>& target_image_spacing,
		const array<ElementType, Dimensions>& target_image_position_end,
		const algorithm_version::grid_search::parameters<ElementType, Dimensions>& grid_search_params,
		const ParamsType& params)
	{
		return gamma_index_minimize_pass_impl(
			gamma_index_output, gamma_index_output_end,
			reference_doses,
			reference_pixel_index,
			reference_image_size,
			reference_image_position,
			reference_image_spacing,
			target_doses,
			target_image_size,
			target_image_position,
			target_image_spacing,
			target_image_position_end,
			grid_search_params,
			params,
			std::make_index_sequence<Dimensions>()
		);
	}

	template<typename ElementType>
	constexpr error_code gamma_index_finalize_pass(view<ElementType> gamma_index_output, view<ElementType> gamma_index_output_end)
	{
		return gamma_index_finalize_pass_impl((std::execution::seq, gamma_index_output, gamma_index_output_end);
	}
}