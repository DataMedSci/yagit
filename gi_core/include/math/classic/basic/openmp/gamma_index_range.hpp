#pragma once

#include <math/classic/basic/openmp/gamma_index_range_impl.hpp>

namespace yagit::core::math
{
	template<typename Type, size_t Dimensions, typename ParamsType>
	constexpr error_code gamma_index(
		algorithm_version::classic,
		execution::parallel_policy,
		view<Type> gamma_index_output, view<Type> gamma_index_output_end,
		const_view<Type> reference_doses,
		array<const_view<Type>, Dimensions> reference_coordinates,
		const_view<Type> target_doses, const_view<Type> target_doses_end,
		array<const_view<Type>, Dimensions> target_coordinates,
		const ParamsType& params
	)
	{
		if (auto ec = classic::basic::openmp::detail::gamma_index_initialize_pass(gamma_index_output, gamma_index_output_end))
			return ec;
		if (auto ec = classic::basic::openmp::detail::gamma_index_minimize_pass(
			gamma_index_output, gamma_index_output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params
		))
			return ec;
		if (auto ec = classic::basic::openmp::detail::gamma_index_finalize_pass(gamma_index_output, gamma_index_output_end))
			return ec;
		return {};
	}
}