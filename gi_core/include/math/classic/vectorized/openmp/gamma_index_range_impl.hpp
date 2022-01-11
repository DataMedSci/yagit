#pragma once

#include <math/classic/vectorized/gamma_index_range_impl.hpp>

namespace yagit::core::math::classic::vectorized::detail
{
	using math::vectorized::LocalOrGlobalVectorGammaIndexParameters;
	using math::vectorized::prepare_vectorized_gi_params;
	using math::vectorized::fast_vector_size_v;

	// parallel unsequenced policy specializations

	template<size_t VectorSize, typename ElementType, size_t Dimensions>
	error_code gamma_index_initialize_pass(execution::parallel_unsequenced_policy policy_params, const output_image_view<ElementType, Dimensions>& gamma_index_output)
	{
		if (is_aligned_to<VectorSize>(std::data(gamma_index_output.output)))
		{
			return generic::detail::gamma_index_initialize_pass_impl<true, VectorSize>(
				policy_params, std::begin(gamma_index_output.output), std::end(gamma_index_output.output)
				);
		}
		else
		{
			return generic::detail::gamma_index_initialize_pass_impl<false, VectorSize>(
				policy_params, std::begin(gamma_index_output.output), std::end(gamma_index_output.output)
				);
		}
	}

	template<
		size_t VectorSize,
		typename ElementType,
		size_t Dimensions,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType0,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType1,
		LocalOrGlobalGammaIndexParameters<ElementType> GIParamsType,
		LocalOrGlobalVectorGammaIndexParameters<ElementType, VectorSize> GIVParamsType
	>
	error_code gamma_index_minimize_pass(
		execution::parallel_unsequenced_policy policy_params,
		const output_image_view<ElementType, Dimensions>& gamma_index_output,
		const InputImageViewType0& reference_image,
		const InputImageViewType1& target_image,
		const algorithm_version::classic::parameters<ElementType, Dimensions>& classic_params,
		const GIParamsType& params,
		const GIVParamsType& paramsv
	)
	{
		constexpr auto is_aligned = [](auto&& cv) {return is_aligned_to<VectorSize>(std::data(cv)); };
		if (is_aligned_to<VectorSize>(std::data(gamma_index_output.output)) &&
			is_aligned_to<VectorSize>(std::data(reference_image.doses)) &&
			std::all_of(std::begin(classic_params.reference_image_coordinates), std::end(classic_params.reference_image_coordinates), is_aligned) &&
			is_aligned_to<VectorSize>(std::data(target_image.doses)) &&
			std::all_of(std::begin(classic_params.target_image_coordinates), std::end(classic_params.target_image_coordinates), is_aligned)
			)
		{
			return generic::detail::gamma_index_minimize_pass_impl<true, VectorSize>(
				policy_params,
				gamma_index_output,
				reference_image,
				target_image,
				classic_params,
				params,
				paramsv,
				std::make_index_sequence<Dimensions>()
				);
		}
		else
		{
			return generic::detail::gamma_index_minimize_pass_impl<false, VectorSize>(
				policy_params,
				gamma_index_output,
				reference_image,
				target_image,
				classic_params,
				params,
				paramsv,
				std::make_index_sequence<Dimensions>()
				);
		}
	}

	template<size_t VectorSize, typename ElementType, size_t Dimensions>
	error_code gamma_index_finalize_pass(execution::parallel_unsequenced_policy policy_params, const output_image_view<ElementType, Dimensions>& gamma_index_output)
	{
		if (is_aligned_to<VectorSize>(std::data(gamma_index_output.output)))
		{
			return generic::detail::gamma_index_finalize_pass_impl<true, VectorSize>(
				policy_params, std::begin(gamma_index_output.output), std::end(gamma_index_output.output)
				);
		}
		else
		{
			return generic::detail::gamma_index_finalize_pass_impl<false, VectorSize>(
				policy_params, std::begin(gamma_index_output.output), std::end(gamma_index_output.output)
				);
		}
	}

	template<
		size_t VectorSize,
		typename ElementType,
		size_t Dimensions,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType0,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType1,
		LocalOrGlobalGammaIndexParameters<ElementType> GIParamsType
	>
	error_code gamma_indexv(
		algorithm_version::classic,
		execution::parallel_unsequenced_policy policy_params,
		const output_image_view<ElementType, Dimensions>& gamma_index_output,
		const InputImageViewType0& reference_image,
		const InputImageViewType1& target_image,
		const algorithm_version::classic::parameters<ElementType, Dimensions>& classic_params,
		const GIParamsType& params
	)
	{
		if (auto ec = gamma_index_initialize_pass<VectorSize>(policy_params, gamma_index_output))
			return ec;
		if (auto ec = gamma_index_minimize_pass<VectorSize>(
			policy_params,
			gamma_index_output,
			reference_image,
			target_image,
			classic_params,
			params,
			prepare_vectorized_gi_params<VectorSize, ElementType>(params)
			))
			return ec;
		if (auto ec = gamma_index_finalize_pass<VectorSize>(policy_params, gamma_index_output))
			return ec;
		return {};
	}

#define DECLARE_CLASSIC_PAR_UNSEQ_GI(ElementType, Dimensions, InputImageViewType0, InputImageViewType1, GIParamsType) \
	error_code gamma_index(\
		algorithm_version::classic _v,\
		execution::parallel_unsequenced_policy policy_params,\
		const output_image_view<ElementType, Dimensions>& gamma_index_output,\
		const InputImageViewType0& reference_image,\
		const InputImageViewType1& target_image,\
		const algorithm_version::classic::parameters<ElementType, Dimensions>& classic_params,\
		const GIParamsType& params\
	)

#define IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(ElementType, Dimensions, InputImageViewType0, InputImageViewType1, GIParamsType) \
	DECLARE_CLASSIC_PAR_UNSEQ_GI(ElementType, Dimensions, InputImageViewType0, InputImageViewType1, GIParamsType)\
	{\
		return vectorized::detail::gamma_indexv<fast_vector_size_v<ElementType>>(\
			_v,\
			policy_params,\
			gamma_index_output,\
			reference_image,\
			target_image,\
			classic_params,\
			params\
			);\
	}

#define UNPACK(...) __VA_ARGS__

#define MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(ElementType, Dimensions, InputImageViewType0, InputImageViewType1, GIParamsType)\
	SIMDPP_MAKE_DISPATCHER(\
		(error_code)\
		(gamma_index)\
		(\
			(algorithm_version::classic) _v,\
			(execution::parallel_unsequenced_policy) policy_params, \
			(const output_image_view<ElementType, Dimensions>&) gamma_index_output, \
			(const InputImageViewType0&) reference_image, \
			(const InputImageViewType1&) target_image,\
			(const algorithm_version::classic::parameters<ElementType, Dimensions>&) classic_params, \
			(const GIParamsType&) params\
		)\
	);

	template<
		typename ElementType,
		size_t Dimensions,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType0,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType1,
		LocalOrGlobalGammaIndexParameters<ElementType> GIParamsType
	>
	IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(ElementType, Dimensions, InputImageViewType0, InputImageViewType1, GIParamsType);

	// explicit specializations for dynamic dispatch
	// -------- float32 --------

	DECLARE_CLASSIC_PAR_UNSEQ_GI(float, 1, UNPACK(input_uniform_image_view<float, 1>), UNPACK(input_uniform_image_view<float, 1>), local_gamma_index_params<float>);
	DECLARE_CLASSIC_PAR_UNSEQ_GI(float, 2, UNPACK(input_uniform_image_view<float, 2>), UNPACK(input_uniform_image_view<float, 2>), local_gamma_index_params<float>);
	DECLARE_CLASSIC_PAR_UNSEQ_GI(float, 3, UNPACK(input_uniform_image_view<float, 3>), UNPACK(input_uniform_image_view<float, 3>), local_gamma_index_params<float>);
	DECLARE_CLASSIC_PAR_UNSEQ_GI(float, 1, UNPACK(input_uniform_image_view<float, 1>), UNPACK(input_uniform_image_view<float, 1>), global_gamma_index_params<float>);
	DECLARE_CLASSIC_PAR_UNSEQ_GI(float, 2, UNPACK(input_uniform_image_view<float, 2>), UNPACK(input_uniform_image_view<float, 2>), global_gamma_index_params<float>);
	DECLARE_CLASSIC_PAR_UNSEQ_GI(float, 3, UNPACK(input_uniform_image_view<float, 3>), UNPACK(input_uniform_image_view<float, 3>), global_gamma_index_params<float>);

	// -------- float64 --------

	DECLARE_CLASSIC_PAR_UNSEQ_GI(double, 1, UNPACK(input_uniform_image_view<double, 1>), UNPACK(input_uniform_image_view<double, 1>), local_gamma_index_params<double>);
	DECLARE_CLASSIC_PAR_UNSEQ_GI(double, 2, UNPACK(input_uniform_image_view<double, 2>), UNPACK(input_uniform_image_view<double, 2>), local_gamma_index_params<double>);
	DECLARE_CLASSIC_PAR_UNSEQ_GI(double, 3, UNPACK(input_uniform_image_view<double, 3>), UNPACK(input_uniform_image_view<double, 3>), local_gamma_index_params<double>);
	DECLARE_CLASSIC_PAR_UNSEQ_GI(double, 1, UNPACK(input_uniform_image_view<double, 1>), UNPACK(input_uniform_image_view<double, 1>), global_gamma_index_params<double>);
	DECLARE_CLASSIC_PAR_UNSEQ_GI(double, 2, UNPACK(input_uniform_image_view<double, 2>), UNPACK(input_uniform_image_view<double, 2>), global_gamma_index_params<double>);
	DECLARE_CLASSIC_PAR_UNSEQ_GI(double, 3, UNPACK(input_uniform_image_view<double, 3>), UNPACK(input_uniform_image_view<double, 3>), global_gamma_index_params<double>);
}