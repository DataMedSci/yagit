#pragma once

#include <math/classic/vectorized/openmp/gamma_index_range_impl.hpp>

namespace yagit::core::math
{
	template<typename Type, size_t Dimensions, typename ParamsType>
	constexpr error_code gamma_index(
		algorithm_version::classic al_v,
		execution::parallel_unsequenced_policy ex_p,
		view<Type> gamma_index_output, view<Type> gamma_index_output_end,
		const_view<Type> reference_doses,
		array<const_view<Type>, Dimensions> reference_coordinates,
		const_view<Type> target_doses, const_view<Type> target_doses_end,
		array<const_view<Type>, Dimensions> target_coordinates,
		const ParamsType& params
	)
	{
		return classic::vectorized::openmp::detail::gamma_index(
			helpers::gi_params<Type, Dimensions, ParamsType, algorithm_version::classic, execution::parallel_unsequenced_policy>{
			gamma_index_output, gamma_index_output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params,
				al_v,
				ex_p
		});
	}

	template<size_t VectorSize, typename Type, size_t Dimensions, typename ParamsType, typename = std::enable_if_t<is_power_of_two_v<VectorSize>>>
	constexpr error_code gamma_index(
		algorithm_version::classic al_v,
		execution::parallel_unsequenced_policy ex_p,
		view<Type> gamma_index_output, view<Type> gamma_index_output_end,
		const_view<Type> reference_doses,
		array<const_view<Type>, Dimensions> reference_coordinates,
		const_view<Type> target_doses, const_view<Type> target_doses_end,
		array<const_view<Type>, Dimensions> target_coordinates,
		const ParamsType& params
	)
	{
		return classic::vectorized::openmp::detail::gamma_index<VectorSize>(
			gamma_index_output, gamma_index_output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params,
			al_v,
			ex_p
			);
	}
}