#pragma once

#include <math/vectorized/common.hpp>
#if !defined(YAGIT_OPENMP)
#include <math/vectorized/gamma_index_range_impl.hpp>
#endif
#include <math/basic/openmp/gamma_index_range_impl.hpp>
#include <math/vectorized/gamma_index_range_impl.hpp>
#include <math/vectorized/gamma_index_single.hpp>
#include <math/basic/gamma_index_single.hpp>

namespace yagit::core::math::vectorized::openmp::detail
{
#if defined(YAGIT_OPENMP)
	template<bool Alignment, unsigned VectorSize, typename Type>
	error_code gamma_index_initialize_pass(view<Type> gamma_index_output, view<Type> gamma_index_output_end)
	{
		vec_t<Type, VectorSize> valueV = simdpp::splat(numeric_limits<Type>::max());
		// vectorized
		long length_outputv = static_cast<long>((gamma_index_output_end - gamma_index_output) / VectorSize);
#pragma omp parallel for
		for (long i = 0; i < length_outputv; ++i)
            vectorized::store<Alignment>(gamma_index_output + i * VectorSize, valueV);
		//fallback
		return basic::openmp::gamma_index_initialize_pass(gamma_index_output + length_outputv * VectorSize, gamma_index_output_end);
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

				if (std::isnan(*gi_output) || *gi_output < static_cast<Type>(1))
					continue;

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
						vec_t<Type, VectorSize> gi = vectorized::gamma_index(reference_dose, target_dose, reference_coords[I]..., ((vec_t<Type, VectorSize>)load<Alignment>(t_coordinates[I]))..., vparams);
						auto intermediate_minimum = simdpp::min(
							gi_minimumv,
							gi
						);

						gi_minimumv = simdpp::blend(gi_minimumv, intermediate_minimum, nan_mask_output);
						if (simdpp::test_bits_any(simdpp::bit_cast<vec_t<Type, VectorSize>>(gi_minimumv < one)))
						{
							skip = true;
							break;
						}
					}

					if (skip)
						gi_minimum = std::min(gi_minimum, vectorized::find_if(gi_minimumv, [](auto v) { return v < static_cast<Type>(1); }));
					else
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
							basic::gamma_index(*ref_doses, *t_doses, (*ref_coords[I])..., (*t_coordinates[I])..., params)
						);
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
            vectorized::store<Alignment>(gamma_index_output + i * VectorSize, simdpp::sqrt((vec_t<Type, VectorSize>)load<Alignment>(gamma_index_output + i * VectorSize)));
		// fallback
		return basic::openmp::gamma_index_finalize_pass(gamma_index_output + length_outputv * VectorSize, gamma_index_output_end);
	}
#else
	using vectorized::detail::gamma_index_initialize_pass;
	using vectorized::detail::gamma_index_minimize_pass;
	using vectorized::detail::gamma_index_finalize_pass;
#endif
	// -------- float32 --------

	error_code gamma_index_initialize_pass(view<float> gamma_index_output, view<float> gamma_index_output_end);
	
	error_code gamma_index_minimize_pass(
		view<float> gamma_index_output, view<float> gamma_index_output_end,
		const_view<float> reference_doses,
		array<const_view<float>, 1> reference_coordinates,
		const_view<float> target_doses, const_view<float> target_doses_end,
		array<const_view<float>, 1> target_coordinates,
		const local_gamma_index_params<float>& params);
	error_code gamma_index_minimize_pass(
		view<float> gamma_index_output, view<float> gamma_index_output_end,
		const_view<float> reference_doses,
		array<const_view<float>, 2> reference_coordinates,
		const_view<float> target_doses, const_view<float> target_doses_end,
		array<const_view<float>, 2> target_coordinates,
		const local_gamma_index_params<float>& params);
	error_code gamma_index_minimize_pass(
		view<float> gamma_index_output, view<float> gamma_index_output_end,
		const_view<float> reference_doses,
		array<const_view<float>, 3> reference_coordinates,
		const_view<float> target_doses, const_view<float> target_doses_end,
		array<const_view<float>, 3> target_coordinates,
		const local_gamma_index_params<float>& params);
	error_code gamma_index_minimize_pass(
		view<float> gamma_index_output, view<float> gamma_index_output_end,
		const_view<float> reference_doses,
		array<const_view<float>, 1> reference_coordinates,
		const_view<float> target_doses, const_view<float> target_doses_end,
		array<const_view<float>, 1> target_coordinates,
		const global_gamma_index_params<float>& params);
	error_code gamma_index_minimize_pass(
		view<float> gamma_index_output, view<float> gamma_index_output_end,
		const_view<float> reference_doses,
		array<const_view<float>, 2> reference_coordinates,
		const_view<float> target_doses, const_view<float> target_doses_end,
		array<const_view<float>, 2> target_coordinates,
		const global_gamma_index_params<float>& params);
	error_code gamma_index_minimize_pass(
		view<float> gamma_index_output, view<float> gamma_index_output_end,
		const_view<float> reference_doses,
		array<const_view<float>, 3> reference_coordinates,
		const_view<float> target_doses, const_view<float> target_doses_end,
		array<const_view<float>, 3> target_coordinates,
		const global_gamma_index_params<float>& params);

	error_code gamma_index_finalize_pass(view<float> gamma_index_output, view<float> gamma_index_output_end);

	// -------- float64 --------

	error_code gamma_index_initialize_pass(view<double> gamma_index_output, view<double> gamma_index_output_end);

	error_code gamma_index_minimize_pass(
		view<double> gamma_index_output, view<double> gamma_index_output_end,
		const_view<double> reference_doses,
		array<const_view<double>, 1> reference_coordinates,
		const_view<double> target_doses, const_view<double> target_doses_end,
		array<const_view<double>, 1> target_coordinates,
		const local_gamma_index_params<double>& params);
	error_code gamma_index_minimize_pass(
		view<double> gamma_index_output, view<double> gamma_index_output_end,
		const_view<double> reference_doses,
		array<const_view<double>, 2> reference_coordinates,
		const_view<double> target_doses, const_view<double> target_doses_end,
		array<const_view<double>, 2> target_coordinates,
		const local_gamma_index_params<double>& params);
	error_code gamma_index_minimize_pass(
		view<double> gamma_index_output, view<double> gamma_index_output_end,
		const_view<double> reference_doses,
		array<const_view<double>, 3> reference_coordinates,
		const_view<double> target_doses, const_view<double> target_doses_end,
		array<const_view<double>, 3> target_coordinates,
		const local_gamma_index_params<double>& params);
	error_code gamma_index_minimize_pass(
		view<double> gamma_index_output, view<double> gamma_index_output_end,
		const_view<double> reference_doses,
		array<const_view<double>, 1> reference_coordinates,
		const_view<double> target_doses, const_view<double> target_doses_end,
		array<const_view<double>, 1> target_coordinates,
		const global_gamma_index_params<double>& params);
	error_code gamma_index_minimize_pass(
		view<double> gamma_index_output, view<double> gamma_index_output_end,
		const_view<double> reference_doses,
		array<const_view<double>, 2> reference_coordinates,
		const_view<double> target_doses, const_view<double> target_doses_end,
		array<const_view<double>, 2> target_coordinates,
		const global_gamma_index_params<double>& params);
	error_code gamma_index_minimize_pass(
		view<double> gamma_index_output, view<double> gamma_index_output_end,
		const_view<double> reference_doses,
		array<const_view<double>, 3> reference_coordinates,
		const_view<double> target_doses, const_view<double> target_doses_end,
		array<const_view<double>, 3> target_coordinates,
		const global_gamma_index_params<double>& params);

	error_code gamma_index_finalize_pass(view<double> gamma_index_output, view<double> gamma_index_output_end);
}