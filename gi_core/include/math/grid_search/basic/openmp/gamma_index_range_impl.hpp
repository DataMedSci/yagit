#pragma once

#include <math/grid_search/basic/gamma_index_range_impl.hpp>

namespace yagit::core::math::grid_search::basic::openmp::detail
{
#if defined(YAGIT_OPENMP)
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
		long length = gamma_index_output_end - gamma_index_output;
#pragma omp parallel for
		for (long i = 0; i < length; i++)
		{
			gamma_index_output[i] = gamma_index_minimize_pass_single_impl(
				reference_doses[i],
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
#else
	using grid_search::basic::detail::gamma_index_minimize_pass_impl;
#endif


	using classic::basic::detail::gamma_index_initialize_pass_impl;
	using classic::basic::detail::gamma_index_finalize_pass_impl;

	template<typename ElementType>
	constexpr error_code gamma_index_initialize_pass(view<ElementType> gamma_index_output, view<ElementType> gamma_index_output_end)
	{
		return gamma_index_initialize_pass_impl(std::execution::par, gamma_index_output, gamma_index_output_end);
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
		return gamma_index_finalize_pass_impl((std::execution::par, gamma_index_output, gamma_index_output_end);
	}
}