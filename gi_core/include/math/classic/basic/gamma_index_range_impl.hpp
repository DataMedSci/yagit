#pragma once

#include <math/single/basic/gamma_index_single.hpp>

namespace yagit::core::math::classic::basic::generic::detail
{
	using single::gamma_index;

	/// <summary>
	/// Gamma index minimization pass analysing single reference image point
	/// </summary>
	/// <returns>Minimum gamma index value calculated against target image</returns>
	template<
		typename ExecutionPolicy,
		typename ElementType,
		size_t Dimensions,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType,
		LocalOrGlobalGammaIndexParameters<ElementType> GIParamsType,
		size_t... I
	>
	constexpr ElementType gamma_index_minimize_pass_single_impl(
		ExecutionPolicy policy_params,
		ElementType reference_dose,
		array<ElementType, Dimensions> reference_coordinates,
		const InputImageViewType& target_image,
		const algorithm_version::classic::parameters<ElementType, Dimensions>& classic_params,
		const GIParamsType& params,
		index_sequence<I...> _i)
	{
		if (std::isnan(reference_dose))
			return reference_dose;

		auto target_doses_it = std::begin(target_image.doses);
		auto target_doses_end = std::end(target_image.doses);
		auto target_coordinates_its = array{ std::begin(classic_params.target_image_coordinates[I])... };

		ElementType min_gi = numeric_limits<ElementType>::max();

		while (target_doses_it != target_doses_end)
		{
			auto target_dose = *target_doses_it++;
			auto target_coordinates = std::array<ElementType, Dimensions>{ *target_coordinates_its[I]++... };

			if (std::isnan(target_dose))
				continue;

			min_gi = std::min(
				min_gi,
				gamma_index(
					reference_dose,
					target_dose,
					reference_coordinates[I]...,
					target_coordinates[I]...,
					params
				)
			);

			if (min_gi <= static_cast<ElementType>(1))
				break;
		}

		return min_gi;
	}

	/// <summary>
	/// Gamma index minimization pass analysing whole reference image
	/// using gamma_index_minimize_pass_single_impl
	/// </summary>
	template<
		typename ExecutionPolicy,
		typename ElementType,
		size_t Dimensions,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType0,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType1,
		LocalOrGlobalGammaIndexParameters<ElementType> GIParamsType,
		size_t... I
	>
	constexpr error_code gamma_index_minimize_pass_impl(
		ExecutionPolicy policy_params,
		const output_image_view<ElementType, Dimensions>& gamma_index_output,
		const InputImageViewType0& reference_image,
		const InputImageViewType1& target_image,
		const algorithm_version::classic::parameters<ElementType, Dimensions>& classic_params,
		const GIParamsType& params,
		index_sequence<I...> _i)
	{
		transform(
			policy_params,
			std::begin(reference_image.doses), std::end(reference_image.doses),
			std::begin(gamma_index_output.output),
			[&](auto&& reference_dose, auto&&... reference_coordinates)
			{
				return gamma_index_minimize_pass_single_impl(
					policy_params,
					reference_dose,
					array{ reference_coordinates... },
					target_image,
					classic_params,
					params,
					_i
				);
			},
			std::begin(classic_params.reference_image_coordinates[I])...
		);

		return {};
	}

	template<typename ExecutionPolicy, typename It>
	constexpr error_code gamma_index_initialize_pass_impl(const ExecutionPolicy& execution_policy, It gi_output_begin, It gi_output_end)
	{
		std::fill(execution_policy, gi_output_begin, gi_output_end, std::numeric_limits<typename std::iterator_traits<It>::value_type>::max());
		return {};
	}

	template<typename ExecutionPolicy, typename It>
	constexpr error_code gamma_index_finalize_pass_impl(const ExecutionPolicy& execution_policy, It gi_output_begin, It gi_output_end)
	{
		std::transform(execution_policy, gi_output_begin, gi_output_end, gi_output_begin, [](auto&& v) {return std::sqrt(v); });
		return {};
	}
}

namespace yagit::core::math::classic::basic::detail
{
	// sequenced policy specializations

	template<typename ElementType, size_t Dimensions>
	constexpr error_code gamma_index_initialize_pass(execution::sequenced_policy policy_params, const output_image_view<ElementType, Dimensions>& gamma_index_output)
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
		execution::sequenced_policy policy_params,
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
	constexpr error_code gamma_index_finalize_pass(execution::sequenced_policy policy_params, const output_image_view<ElementType, Dimensions>& gamma_index_output)
	{
		return generic::detail::gamma_index_finalize_pass_impl(policy_params, std::begin(gamma_index_output.output), std::end(gamma_index_output.output));
	}
}