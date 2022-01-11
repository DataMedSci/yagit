#pragma once

#include <math/classic/basic/gamma_index_range_impl.hpp>

namespace yagit::core::math::classic::basic::detail
{
	// parallel policy specializations

	template<typename ElementType, size_t Dimensions>
	constexpr error_code gamma_index_initialize_pass(execution::parallel_policy policy_params, const output_image_view<ElementType, Dimensions>& gamma_index_output)
	{
		return generic::detail::gamma_index_initialize_pass_impl(policy_params, std::begin(gamma_index_output.output), std::end(gamma_index_output.output));
	}

	template<
		typename ElementType,
		size_t Dimensions,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType0,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType1,
		LocalOrGlobalGammaIndexParameters<ElementType> GIParamsType
	>
	constexpr error_code gamma_index_minimize_pass(
		execution::parallel_policy policy_params,
		const output_image_view<ElementType, Dimensions>& gamma_index_output,
		const InputImageViewType0& reference_image,
		const InputImageViewType1& target_image,
		const algorithm_version::classic::parameters<ElementType, Dimensions>& classic_params,
		const GIParamsType& params
	)
	{
		return generic::detail::gamma_index_minimize_pass_impl(
			policy_params,
			gamma_index_output,
			reference_image,
			target_image,
			classic_params,
			params,
			std::make_index_sequence<Dimensions>()
		);
	}

	template<typename ElementType, size_t Dimensions>
	constexpr error_code gamma_index_finalize_pass(execution::parallel_policy policy_params, const output_image_view<ElementType, Dimensions>& gamma_index_output)
	{
		return generic::detail::gamma_index_finalize_pass_impl(policy_params, std::begin(gamma_index_output.output), std::end(gamma_index_output.output));
	}
}