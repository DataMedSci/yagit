#pragma once

#include <math/classic/vectorized/openmp/gamma_index_range_impl.hpp>

namespace yagit::core::math
{
	template<
		typename ElementType,
		size_t Dimensions,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType0,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType1,
		LocalOrGlobalGammaIndexParameters<ElementType> GIParamsType
	>
	constexpr error_code gamma_index(
		algorithm_version::classic _v,
		execution::parallel_unsequenced_policy policy_params,
		const output_image_view<ElementType, Dimensions>& gamma_index_output,
		const InputImageViewType0& reference_image,
		const InputImageViewType1& target_image,
		const algorithm_version::classic::parameters<ElementType, Dimensions>& classic_params,
		const GIParamsType& params
	)
	{
		return classic::vectorized::detail::gamma_index(
			_v,
			policy_params,
			gamma_index_output,
			reference_image,
			target_image,
			classic_params,
			prepare_gi_params<ElementType>(params)
		);
	}

	template<
		size_t VectorSize,
		typename ElementType,
		size_t Dimensions,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType0,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType1,
		LocalOrGlobalGammaIndexParameters<ElementType> GIParamsType
	>
	constexpr error_code gamma_index(
		algorithm_version::classic _v,
		execution::parallel_unsequenced_policy policy_params,
		const output_image_view<ElementType, Dimensions>& gamma_index_output,
		const InputImageViewType0& reference_image,
		const InputImageViewType1& target_image,
		const algorithm_version::classic::parameters<ElementType, Dimensions>& classic_params,
		const GIParamsType& params
	)
	{
		return classic::vectorized::detail::gamma_indexv<VectorSize>(
			_v,
			policy_params,
			gamma_index_output,
			reference_image,
			target_image,
			classic_params,
			prepare_gi_params<ElementType>(params)
		);
	}
}