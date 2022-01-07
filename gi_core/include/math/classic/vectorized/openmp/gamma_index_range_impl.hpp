#pragma once

#include <math/single/vectorized/gamma_index_single.hpp>
#include <math/single/basic/gamma_index_single.hpp>
#if !defined(YAGIT_OPENMP)
#include <math/classic/vectorized/gamma_index_range_impl.hpp>
#endif
#include <math/classic/basic/openmp/gamma_index_range_impl.hpp>

namespace yagit::core::math::classic::vectorized::openmp::detail
{
	using single::vectorized::vec_t;
	using single::vectorized::load;
	using single::vectorized::store;
	using single::vectorized::find_if;
	using single::vectorized::params_vec_t;
	using single::vectorized::gamma_index;
	using single::basic::gamma_index;
	using single::vectorized::fast_vector_size_v;

#if defined(YAGIT_OPENMP)
	template<bool Alignment, unsigned VectorSize, typename Type>
	error_code gamma_index_initialize_pass(view<Type> gamma_index_output, view<Type> gamma_index_output_end)
	{
		vec_t<Type, VectorSize> valueV = simdpp::splat(numeric_limits<Type>::max());
		// vectorized
		long length_outputv = static_cast<long>((gamma_index_output_end - gamma_index_output) / VectorSize);
#pragma omp parallel for
		for (long i = 0; i < length_outputv; ++i)
            store<Alignment>(gamma_index_output + i * VectorSize, valueV);
		//fallback
		return basic::openmp::detail::gamma_index_initialize_pass(gamma_index_output + length_outputv * VectorSize, gamma_index_output_end);
	}

	template<bool Alignment, unsigned VectorSize, typename Type, size_t Dimensions, typename ParamsType, size_t... I>
	error_code gamma_index_minimize_pass(
		view<Type> gamma_index_output, view<Type> gamma_index_output_end,
		const_view<Type> reference_doses,
		array<const_view<Type>, Dimensions> reference_coordinates,
		const_view<Type> target_doses, const_view<Type> target_doses_end,
		array<const_view<Type>, Dimensions> target_coordinates,
		const ParamsType& params,
		index_sequence<I...>)
	{
		// vectorized
		{
			auto vparams = params_vec_t<ParamsType, VectorSize>(params);

			long length_outputv = static_cast<long>(gamma_index_output_end - gamma_index_output);
			ptrdiff_t length_targetv = (target_doses_end - target_doses) / VectorSize;

#pragma omp parallel for
			for (long i = 0; i < length_outputv; ++i)
			{
				auto gi_output = gamma_index_output + i;
				auto ref_doses = reference_doses + i;
				array<const_view<Type>, Dimensions> ref_coords = { (reference_coordinates[I] + i)... };

				if (std::isnan(*ref_doses))
				{
					*gi_output = *ref_doses;
					continue;
				}

				Type gi_minimum = numeric_limits<Type>::max();

				const_view<Type> t_doses = target_doses;
				array<const_view<Type>, Dimensions> t_coordinates = target_coordinates;
				bool skip = false;
				// vectorized
				{
					vec_t<Type, VectorSize> reference_dose = simdpp::load_splat<vec_t<Type, VectorSize>>(ref_doses);
					array<vec_t<Type, VectorSize>, Dimensions> reference_coords = { simdpp::load_splat<vec_t<Type, VectorSize>>(ref_coords[I])... };

					vec_t<Type, VectorSize> gi_minimumv = simdpp::splat(numeric_limits<Type>::max());
					vec_t<Type, VectorSize> one = simdpp::splat(static_cast<Type>(1));

					for (ptrdiff_t count_processed_target = 0;
						count_processed_target < length_targetv;
						t_doses += VectorSize, ((t_coordinates[I] += VectorSize), ...), ++count_processed_target)
					{
						vec_t<Type, VectorSize> target_dose = load<Alignment>(t_doses);
						auto nan_mask_output = simdpp::isnan(target_dose);
						vec_t<Type, VectorSize> gi = gamma_index(reference_dose, target_dose, reference_coords[I]..., ((vec_t<Type, VectorSize>)load<Alignment>(t_coordinates[I]))..., vparams);
						auto intermediate_minimum = simdpp::min(
							gi_minimumv,
							gi
						);

						gi_minimumv = simdpp::blend(gi_minimumv, intermediate_minimum, nan_mask_output);
						if (simdpp::test_bits_any(simdpp::bit_cast<vec_t<Type, VectorSize>>(gi_minimumv <= one)))
						{
							skip = true;
							break;
						}
					}

					gi_minimum = std::min(gi_minimum, simdpp::reduce_min(gi_minimumv));
				}
				// fallback
				if (!skip)
				{
					for (;
						t_doses != target_doses_end;
						++t_doses, ((++t_coordinates[I]), ...))
					{
						if (std::isnan(*t_doses))
							continue;

						gi_minimum = std::min(
							gi_minimum,
							gamma_index(*ref_doses, *t_doses, (*ref_coords[I])..., (*t_coordinates[I])..., params)
						);

						if (gi_minimum <= static_cast<Type>(1))
							break;
					}
				}

				*gi_output = std::min(*gi_output, gi_minimum);
			}
		}
		return {};
	}

	template<bool Alignment, unsigned VectorSize, typename Type>
	error_code gamma_index_finalize_pass(view<Type> gamma_index_output, view<Type> gamma_index_output_end)
	{
		// vectorized
		long length_outputv = static_cast<long>((gamma_index_output_end - gamma_index_output) / VectorSize);
#pragma omp parallel for
		for (long i = 0; i < length_outputv; ++i)
            store<Alignment>(gamma_index_output + i * VectorSize, simdpp::sqrt((vec_t<Type, VectorSize>)load<Alignment>(gamma_index_output + i * VectorSize)));
		// fallback
		return basic::openmp::detail::gamma_index_finalize_pass(gamma_index_output + length_outputv * VectorSize, gamma_index_output_end);
	}
#else
	using classic::vectorized::detail::gamma_index_initialize_pass;
	using classic::vectorized::detail::gamma_index_minimize_pass;
	using classic::vectorized::detail::gamma_index_finalize_pass;
#endif

	template<size_t VectorSize, typename Type>
	error_code gamma_index_initialize_pass(view<Type> gamma_index_output, view<Type> gamma_index_output_end)
	{
		if (is_aligned_to<VectorSize>(gamma_index_output))
		{
			return gamma_index_initialize_pass<true, VectorSize>(gamma_index_output, gamma_index_output_end);
		}
		else
		{
			return gamma_index_initialize_pass<false, VectorSize>(gamma_index_output, gamma_index_output_end);
		}
	}

	template<size_t VectorSize, typename Type, size_t Dimensions, typename ParamsType>
	error_code gamma_index_minimize_pass(
		view<Type> gamma_index_output, view<Type> gamma_index_output_end,
		const_view<Type> reference_doses,
		array<const_view<Type>, Dimensions> reference_coordinates,
		const_view<Type> target_doses, const_view<Type> target_doses_end,
		array<const_view<Type>, Dimensions> target_coordinates,
		const ParamsType& params)
	{
		constexpr auto is_aligned = [](auto&& cv) {return is_aligned_to<VectorSize>(cv); };
		if (is_aligned_to<VectorSize>(gamma_index_output) &&
			is_aligned_to<VectorSize>(reference_doses) &&
			std::all_of(std::begin(reference_coordinates), std::end(reference_coordinates), is_aligned) &&
			is_aligned_to<VectorSize>(target_doses) &&
			std::all_of(std::begin(target_coordinates), std::end(target_coordinates), is_aligned)
			)
		{
			return gamma_index_minimize_pass<true, VectorSize>(
				gamma_index_output, gamma_index_output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params, std::make_index_sequence<Dimensions>());
		}
		else
		{
			return gamma_index_minimize_pass<false, VectorSize>(
				gamma_index_output, gamma_index_output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params, std::make_index_sequence<Dimensions>());
		}
	}

	template<size_t VectorSize, typename Type>
	error_code gamma_index_finalize_pass(view<Type> gamma_index_output, view<Type> gamma_index_output_end)
	{
		if (is_aligned_to<VectorSize>(gamma_index_output))
		{
			return gamma_index_finalize_pass<true, VectorSize>(gamma_index_output, gamma_index_output_end);
		}
		else
		{
			return gamma_index_finalize_pass<false, VectorSize>(gamma_index_output, gamma_index_output_end);
		}
	}

	template<size_t VectorSize, typename Type, size_t Dimensions, typename ParamsType>
	error_code gamma_index(
		view<Type> gamma_index_output, view<Type> gamma_index_output_end,
		const_view<Type> reference_doses,
		array<const_view<Type>, Dimensions> reference_coordinates,
		const_view<Type> target_doses, const_view<Type> target_doses_end,
		array<const_view<Type>, Dimensions> target_coordinates,
		const ParamsType& params)
	{
		if (auto ec = gamma_index_initialize_pass<VectorSize>(gamma_index_output, gamma_index_output_end))
			return ec;
		if (auto ec = gamma_index_minimize_pass<VectorSize>(
			gamma_index_output, gamma_index_output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params
			))
			return ec;
		if (auto ec = gamma_index_finalize_pass<VectorSize>(gamma_index_output, gamma_index_output_end))
			return ec;
		return {};
	}

#define IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(Type, Dimensions, ParamsType) \
	error_code gamma_index(const helpers::gi_params<Type, Dimensions, ParamsType, algorithm_version::classic, execution::parallel_unsequenced_policy>& params)\
	{\
		return vectorized::openmp::detail::gamma_index<fast_vector_size_v<Type>>(\
			params.gamma_index_output, params.gamma_index_output_end,\
			params.reference_doses,\
			params.reference_coordinates,\
			params.target_doses, params.target_doses_end,\
			params.target_coordinates,\
			params.params\
			);\
	}

	template<typename Type, size_t Dimensions, typename ParamsType>
	IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(Type, Dimensions, ParamsType);

#define MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(Type, Dimensions, ParamsType)\
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index)(\
		(const helpers::gi_params<Type, Dimensions, ParamsType, algorithm_version::classic, execution::parallel_unsequenced_policy>&) params)\
	);

	// explicit specializations for dynamic dispatch
	// -------- float32 --------

	error_code gamma_index(const helpers::gi_params<float, 1, local_gamma_index_params<float>, algorithm_version::classic, execution::parallel_unsequenced_policy>& params);
	error_code gamma_index(const helpers::gi_params<float, 2, local_gamma_index_params<float>, algorithm_version::classic, execution::parallel_unsequenced_policy>& params);
	error_code gamma_index(const helpers::gi_params<float, 3, local_gamma_index_params<float>, algorithm_version::classic, execution::parallel_unsequenced_policy>& params);
	error_code gamma_index(const helpers::gi_params<float, 1, global_gamma_index_params<float>, algorithm_version::classic, execution::parallel_unsequenced_policy>& params);
	error_code gamma_index(const helpers::gi_params<float, 2, global_gamma_index_params<float>, algorithm_version::classic, execution::parallel_unsequenced_policy>& params);
	error_code gamma_index(const helpers::gi_params<float, 3, global_gamma_index_params<float>, algorithm_version::classic, execution::parallel_unsequenced_policy>& params);

	// -------- float64 --------

	error_code gamma_index(const helpers::gi_params<double, 1, local_gamma_index_params<double>, algorithm_version::classic, execution::parallel_unsequenced_policy>& params);
	error_code gamma_index(const helpers::gi_params<double, 2, local_gamma_index_params<double>, algorithm_version::classic, execution::parallel_unsequenced_policy>& params);
	error_code gamma_index(const helpers::gi_params<double, 3, local_gamma_index_params<double>, algorithm_version::classic, execution::parallel_unsequenced_policy>& params);
	error_code gamma_index(const helpers::gi_params<double, 1, global_gamma_index_params<double>, algorithm_version::classic, execution::parallel_unsequenced_policy>& params);
	error_code gamma_index(const helpers::gi_params<double, 2, global_gamma_index_params<double>, algorithm_version::classic, execution::parallel_unsequenced_policy>& params);
	error_code gamma_index(const helpers::gi_params<double, 3, global_gamma_index_params<double>, algorithm_version::classic, execution::parallel_unsequenced_policy>& params);
}