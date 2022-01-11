#pragma once

#include <math/classic/basic/openmp/gamma_index_range_impl.hpp>

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
			algorithm_version::classic,
			execution::parallel_policy policy_params,
			const output_image_view<ElementType, Dimensions>& gamma_index_output,
			const InputImageViewType0& reference_image,
			const InputImageViewType1& target_image,
			const algorithm_version::classic::parameters<ElementType, Dimensions>& classic_params,
			const GIParamsType& params
		)
	{
		if (auto ec = classic::basic::detail::gamma_index_initialize_pass(policy_params, gamma_index_output))
			return ec;
		if (auto ec = classic::basic::detail::gamma_index_minimize_pass(
			policy_params,
			gamma_index_output,
			reference_image,
			target_image,
			classic_params,
			prepare_gi_params<ElementType>(params)
		))
			return ec;
		if (auto ec = classic::basic::detail::gamma_index_finalize_pass(policy_params, gamma_index_output))
			return ec;
		return {};
	}
}