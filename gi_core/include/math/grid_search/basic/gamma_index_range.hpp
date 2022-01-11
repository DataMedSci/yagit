#pragma once

#include <math/grid_search/basic/gamma_index_range_impl.hpp>

namespace yagit::core::math
{
	template<typename ElementType, size_t Dimensions, typename ParamsType>
	constexpr error_code gamma_index(
		algorithm_version::grid_search gs_params,
		execution::sequenced_policy,
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
		const ParamsType& params
	)
	{
		if (auto ec = grid_search::basic::detail::gamma_index_initialize_pass(gamma_index_output, gamma_index_output_end))
			return ec;
		if (auto ec = grid_search::basic::detail::gamma_index_minimize_pass(
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
			params
		))
			return ec;
		if (auto ec = grid_search::basic::detail::gamma_index_finalize_pass(gamma_index_output, gamma_index_output_end))
			return ec;
		return {};
	}
}