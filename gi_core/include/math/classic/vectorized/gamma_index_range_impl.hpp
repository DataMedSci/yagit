#pragma once

#include <math/single/vectorized/gamma_index_single.hpp>
#include <math/single/basic/gamma_index_single.hpp>
#include <math/classic/basic/gamma_index_range_impl.hpp>

namespace yagit::core::math::classic::vectorized::generic::detail
{
	using math::vectorized::LocalOrGlobalVectorGammaIndexParameters;
	using math::vectorized::vec_t;

	using math::vectorized::load;
	using math::vectorized::store;
	using math::vectorized::single::gamma_index;

	using math::single::gamma_index;

	/// <summary>
	/// Gamma index minimization pass analysing single reference image point with vectorization
	/// </summary>
	/// <returns>Minimum gamma index value calculated against target image</returns>
	template<
		bool Alignment,
		size_t VectorSize,
		typename ExecutionPolicy,
		typename ElementType,
		size_t Dimensions,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType,
		LocalOrGlobalGammaIndexParameters<ElementType> GIParamsType,
		LocalOrGlobalVectorGammaIndexParameters<ElementType, VectorSize> GIVParamsType,
		size_t... I
	>
	ElementType gamma_index_minimize_pass_single_impl(
		ExecutionPolicy policy_params,
		ElementType reference_dose,
		array<ElementType, Dimensions> reference_coordinates,
		const InputImageViewType& target_image,
		const algorithm_version::classic::parameters<ElementType, Dimensions>& classic_params,
		const GIParamsType& params,
		const GIVParamsType& paramsv,
		index_sequence<I...> _i)
	{
		if (std::isnan(reference_dose))
			return reference_dose;

		auto target_doses_it = std::begin(target_image.doses);
		auto target_doses_end = std::end(target_image.doses);
		auto target_coordinates_its = array{ std::begin(classic_params.target_image_coordinates[I])... };

		ElementType min_gi = numeric_limits<ElementType>::max();

		auto vectorized = [&]()
		{
			// target image length in vector size (rounded down)
			ptrdiff_t length_targetv = (target_doses_end - target_doses_it) / VectorSize;

			auto reference_dosev = simdpp::load_splat<vec_t<ElementType, VectorSize>>(&reference_dose);
			auto reference_coordsv = array{
				simdpp::load_splat<vec_t<ElementType, VectorSize>>(&reference_coordinates[I])...
			};

			vec_t<ElementType, VectorSize> min_giv = simdpp::splat(numeric_limits<ElementType>::max());
			vec_t<ElementType, VectorSize> onev = simdpp::splat(static_cast<ElementType>(1));

			for (ptrdiff_t count_processed_target = 0;
				count_processed_target < length_targetv;
				target_doses_it += VectorSize, ((target_coordinates_its[I] += VectorSize), ...), ++count_processed_target)
			{
				vec_t<ElementType, VectorSize> target_dosev = load<Alignment>(target_doses_it);
				auto target_coordsv = array{
					load<Alignment>(target_coordinates_its[I])...
				};

				auto nan_mask_outputv = simdpp::isnan(target_dosev);

				auto current_gi = gamma_index<ElementType, VectorSize>(
					reference_dosev,
					target_dosev,
					reference_coordsv[I]...,
					target_coordsv[I]...,
					paramsv
				);

				auto intermediate_gi_minimumv = simdpp::min(min_giv, current_gi);

				min_giv = simdpp::blend(min_giv, intermediate_gi_minimumv, nan_mask_outputv);

				if (simdpp::test_bits_any(simdpp::bit_cast<vec_t<ElementType, VectorSize>>(min_giv <= onev)))
				{
					min_gi = std::min(min_gi, simdpp::reduce_min(min_giv));
					return true;
				}
			}

			min_gi = std::min(min_gi, simdpp::reduce_min(min_giv));
			return false;
		};
		auto fallback = [&]()
		{
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
		};
		
		if(!vectorized())
			fallback();

		return min_gi;
	}

	/// <summary>
	/// Gamma index minimization pass analysing whole reference image
	/// using gamma_index_minimize_pass_single_impl with vectorization
	/// </summary>
	template<
		bool Alignment,
		size_t VectorSize,
		typename ExecutionPolicy,
		typename ElementType,
		size_t Dimensions,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType0,
		AnyInputImageView<ElementType, Dimensions> InputImageViewType1,
		LocalOrGlobalGammaIndexParameters<ElementType> GIParamsType,
		LocalOrGlobalVectorGammaIndexParameters<ElementType, VectorSize> GIVParamsType,
		size_t... I
	>
	error_code gamma_index_minimize_pass_impl(
		ExecutionPolicy policy_params,
		const output_image_view<ElementType, Dimensions>& gamma_index_output,
		const InputImageViewType0& reference_image,
		const InputImageViewType1& target_image,
		const algorithm_version::classic::parameters<ElementType, Dimensions>& classic_params,
		const GIParamsType& params,
		const GIVParamsType& paramsv,
		index_sequence<I...> _i)
	{
		transform(
			policy_params,
			std::begin(reference_image.doses), std::end(reference_image.doses),
			std::begin(gamma_index_output.output),
			[&](auto&& reference_dose, auto&&... reference_coordinates)
			{
				return gamma_index_minimize_pass_single_impl<Alignment, VectorSize>(
					policy_params,
					reference_dose,
					array<ElementType, Dimensions>{ reference_coordinates... },
					target_image,
					classic_params,
					params,
					paramsv,
					_i
				);
			},
			std::begin(classic_params.reference_image_coordinates[I])...
		);

		return {};
	}

	template<bool Alignment, unsigned VectorSize, typename ExecutionPolicy, typename It>
	error_code gamma_index_initialize_pass_impl(const ExecutionPolicy& execution_policy, It gi_output_begin, It gi_output_end)
	{
		return basic::generic::detail::gamma_index_initialize_pass_impl(execution_policy, gi_output_begin, gi_output_end);
	}

	template<bool Alignment, unsigned VectorSize, typename ExecutionPolicy, typename It>
	error_code gamma_index_finalize_pass_impl(const ExecutionPolicy& execution_policy, It gi_output_begin, It gi_output_end)
	{
		return basic::generic::detail::gamma_index_finalize_pass_impl(execution_policy, gi_output_begin, gi_output_end);
	}
}

namespace yagit::core::math::classic::vectorized::detail
{
	using math::vectorized::LocalOrGlobalVectorGammaIndexParameters;
	using math::vectorized::prepare_vectorized_gi_params;
	using math::vectorized::fast_vector_size_v;

	// unsequenced policy specializations

	template<size_t VectorSize, typename ElementType, size_t Dimensions>
	error_code gamma_index_initialize_pass(execution::unsequenced_policy policy_params, const output_image_view<ElementType, Dimensions>& gamma_index_output)
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
		execution::unsequenced_policy policy_params,
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
	error_code gamma_index_finalize_pass(execution::unsequenced_policy policy_params, const output_image_view<ElementType, Dimensions>& gamma_index_output)
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
		execution::unsequenced_policy policy_params,
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

#define DECLARE_CLASSIC_UNSEQ_GI(ElementType, Dimensions, InputImageViewType0, InputImageViewType1, GIParamsType) \
	error_code gamma_index(\
		algorithm_version::classic _v,\
		execution::unsequenced_policy policy_params,\
		const output_image_view<ElementType, Dimensions>& gamma_index_output,\
		const InputImageViewType0& reference_image,\
		const InputImageViewType1& target_image,\
		const algorithm_version::classic::parameters<ElementType, Dimensions>& classic_params,\
		const GIParamsType& params\
	)

#define IMPLEMENT_CLASSIC_UNSEQ_GI(ElementType, Dimensions, InputImageViewType0, InputImageViewType1, GIParamsType) \
	DECLARE_CLASSIC_UNSEQ_GI(ElementType, Dimensions, InputImageViewType0, InputImageViewType1, GIParamsType)\
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

#define MAKE_CLASSIC_UNSEQ_DISPATCHER(ElementType, Dimensions, InputImageViewType0, InputImageViewType1, GIParamsType)\
	SIMDPP_MAKE_DISPATCHER(\
		(error_code)\
		(gamma_index)\
		(\
			(algorithm_version::classic) _v,\
			(execution::unsequenced_policy) policy_params, \
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
	IMPLEMENT_CLASSIC_UNSEQ_GI(ElementType, Dimensions, InputImageViewType0, InputImageViewType1, GIParamsType);

	// explicit specializations for dynamic dispatch
	// -------- float32 --------

	DECLARE_CLASSIC_UNSEQ_GI(float, 1, UNPACK(input_uniform_image_view<float, 1>), UNPACK(input_uniform_image_view<float, 1>), local_gamma_index_params<float>);
	DECLARE_CLASSIC_UNSEQ_GI(float, 2, UNPACK(input_uniform_image_view<float, 2>), UNPACK(input_uniform_image_view<float, 2>), local_gamma_index_params<float>);
	DECLARE_CLASSIC_UNSEQ_GI(float, 3, UNPACK(input_uniform_image_view<float, 3>), UNPACK(input_uniform_image_view<float, 3>), local_gamma_index_params<float>);
	DECLARE_CLASSIC_UNSEQ_GI(float, 1, UNPACK(input_uniform_image_view<float, 1>), UNPACK(input_uniform_image_view<float, 1>), global_gamma_index_params<float>);
	DECLARE_CLASSIC_UNSEQ_GI(float, 2, UNPACK(input_uniform_image_view<float, 2>), UNPACK(input_uniform_image_view<float, 2>), global_gamma_index_params<float>);
	DECLARE_CLASSIC_UNSEQ_GI(float, 3, UNPACK(input_uniform_image_view<float, 3>), UNPACK(input_uniform_image_view<float, 3>), global_gamma_index_params<float>);

	// -------- float64 --------

	DECLARE_CLASSIC_UNSEQ_GI(double, 1, UNPACK(input_uniform_image_view<double, 1>), UNPACK(input_uniform_image_view<double, 1>), local_gamma_index_params<double>);
	DECLARE_CLASSIC_UNSEQ_GI(double, 2, UNPACK(input_uniform_image_view<double, 2>), UNPACK(input_uniform_image_view<double, 2>), local_gamma_index_params<double>);
	DECLARE_CLASSIC_UNSEQ_GI(double, 3, UNPACK(input_uniform_image_view<double, 3>), UNPACK(input_uniform_image_view<double, 3>), local_gamma_index_params<double>);
	DECLARE_CLASSIC_UNSEQ_GI(double, 1, UNPACK(input_uniform_image_view<double, 1>), UNPACK(input_uniform_image_view<double, 1>), global_gamma_index_params<double>);
	DECLARE_CLASSIC_UNSEQ_GI(double, 2, UNPACK(input_uniform_image_view<double, 2>), UNPACK(input_uniform_image_view<double, 2>), global_gamma_index_params<double>);
	DECLARE_CLASSIC_UNSEQ_GI(double, 3, UNPACK(input_uniform_image_view<double, 3>), UNPACK(input_uniform_image_view<double, 3>), global_gamma_index_params<double>);
}