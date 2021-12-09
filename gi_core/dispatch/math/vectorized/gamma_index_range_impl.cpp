// This file is a dynamic-dispatch file used by libsimdpp
// http://p12tic.github.io/libsimdpp/v2.2-dev/libsimdpp/w/arch/dispatch.html
// The is so long because of the fact that each of the function differ only
// sligthly in arguments (float/double 1/2/3 dimensions) and cannot be 
// simplified into a template function due to dynamic-dispatch limitations

#include <math/vectorized/gamma_index_range_impl.hpp>

#include <simdpp/dispatch/get_arch_raw_cpuid.h>
#define SIMDPP_USER_ARCH_INFO ::simdpp::get_arch_raw_cpuid()

namespace yagit::core::math::vectorized::detail
{
	namespace SIMDPP_ARCH_NAMESPACE
	{
		// -------- float32 --------

		constexpr size_t float32_VectorSize = SIMDPP_FAST_FLOAT32_SIZE;

		error_code gamma_index_initialize_pass(view<float> gamma_index_output, view<float> gamma_index_output_end)
		{
			if (is_aligned_to<float32_VectorSize>(gamma_index_output))
			{
				return detail::gamma_index_initialize_pass<true, float32_VectorSize>(gamma_index_output, gamma_index_output_end);
			}
			else
			{
				return detail::gamma_index_initialize_pass<false, float32_VectorSize>(gamma_index_output, gamma_index_output_end);
			}
		}

		error_code gamma_index_minimize_pass(
			view<float> gamma_index_output, view<float> gamma_index_output_end,
			const_view<float> reference_doses,
			array<const_view<float>, 1> reference_coordinates,
			const_view<float> target_doses, const_view<float> target_doses_end,
			array<const_view<float>, 1> target_coordinates,
			const local_gamma_index_params<float>& params)
		{
			if (is_aligned_to<float32_VectorSize>(gamma_index_output) &&
				is_aligned_to<float32_VectorSize>(reference_doses) &&
				is_aligned_to<float32_VectorSize>(reference_coordinates[0]) &&
				is_aligned_to<float32_VectorSize>(target_doses) &&
				is_aligned_to<float32_VectorSize>(target_coordinates[0])
				)
			{
				return detail::gamma_index_minimize_pass<true, float32_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<1>());
			}
			else
			{
				return detail::gamma_index_minimize_pass<false, float32_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<1>());
			}
		}
		error_code gamma_index_minimize_pass(
			view<float> gamma_index_output, view<float> gamma_index_output_end,
			const_view<float> reference_doses,
			array<const_view<float>, 2> reference_coordinates,
			const_view<float> target_doses, const_view<float> target_doses_end,
			array<const_view<float>, 2> target_coordinates,
			const local_gamma_index_params<float>& params)
		{
			if (is_aligned_to<float32_VectorSize>(gamma_index_output) &&
				is_aligned_to<float32_VectorSize>(reference_doses) &&
				is_aligned_to<float32_VectorSize>(reference_coordinates[0]) &&
				is_aligned_to<float32_VectorSize>(reference_coordinates[1]) &&
				is_aligned_to<float32_VectorSize>(target_doses) &&
				is_aligned_to<float32_VectorSize>(target_coordinates[0]) &&
				is_aligned_to<float32_VectorSize>(target_coordinates[1])
				)
			{
				return detail::gamma_index_minimize_pass<true, float32_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<2>());
			}
			else
			{
				return detail::gamma_index_minimize_pass<false, float32_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<2>());
			}
		}
		error_code gamma_index_minimize_pass(
			view<float> gamma_index_output, view<float> gamma_index_output_end,
			const_view<float> reference_doses,
			array<const_view<float>, 3> reference_coordinates,
			const_view<float> target_doses, const_view<float> target_doses_end,
			array<const_view<float>, 3> target_coordinates,
			const local_gamma_index_params<float>& params)
		{
			if (is_aligned_to<float32_VectorSize>(gamma_index_output) &&
				is_aligned_to<float32_VectorSize>(reference_doses) &&
				is_aligned_to<float32_VectorSize>(reference_coordinates[0]) &&
				is_aligned_to<float32_VectorSize>(reference_coordinates[1]) &&
				is_aligned_to<float32_VectorSize>(reference_coordinates[2]) &&
				is_aligned_to<float32_VectorSize>(target_doses) &&
				is_aligned_to<float32_VectorSize>(target_coordinates[0]) &&
				is_aligned_to<float32_VectorSize>(target_coordinates[1]) &&
				is_aligned_to<float32_VectorSize>(target_coordinates[2])
				)
			{
				return detail::gamma_index_minimize_pass<true, float32_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<3>());
			}
			else
			{
				return detail::gamma_index_minimize_pass<false, float32_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<3>());
			}
		}
		error_code gamma_index_minimize_pass(
			view<float> gamma_index_output, view<float> gamma_index_output_end,
			const_view<float> reference_doses,
			array<const_view<float>, 1> reference_coordinates,
			const_view<float> target_doses, const_view<float> target_doses_end,
			array<const_view<float>, 1> target_coordinates,
			const global_gamma_index_params<float>& params)
		{
			if (is_aligned_to<float32_VectorSize>(gamma_index_output) &&
				is_aligned_to<float32_VectorSize>(reference_doses) &&
				is_aligned_to<float32_VectorSize>(reference_coordinates[0]) &&
				is_aligned_to<float32_VectorSize>(target_doses) &&
				is_aligned_to<float32_VectorSize>(target_coordinates[0])
				)
			{
				return detail::gamma_index_minimize_pass<true, float32_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<1>());
			}
			else
			{
				return detail::gamma_index_minimize_pass<false, float32_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<1>());
			}
		}
		error_code gamma_index_minimize_pass(
			view<float> gamma_index_output, view<float> gamma_index_output_end,
			const_view<float> reference_doses,
			array<const_view<float>, 2> reference_coordinates,
			const_view<float> target_doses, const_view<float> target_doses_end,
			array<const_view<float>, 2> target_coordinates,
			const global_gamma_index_params<float>& params)
		{
			if (is_aligned_to<float32_VectorSize>(gamma_index_output) &&
				is_aligned_to<float32_VectorSize>(reference_doses) &&
				is_aligned_to<float32_VectorSize>(reference_coordinates[0]) &&
				is_aligned_to<float32_VectorSize>(reference_coordinates[1]) &&
				is_aligned_to<float32_VectorSize>(target_doses) &&
				is_aligned_to<float32_VectorSize>(target_coordinates[0]) &&
				is_aligned_to<float32_VectorSize>(target_coordinates[1])
				)
			{
				return detail::gamma_index_minimize_pass<true, float32_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<2>());
			}
			else
			{
				return detail::gamma_index_minimize_pass<false, float32_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<2>());
			}
		}
		error_code gamma_index_minimize_pass(
			view<float> gamma_index_output, view<float> gamma_index_output_end,
			const_view<float> reference_doses,
			array<const_view<float>, 3> reference_coordinates,
			const_view<float> target_doses, const_view<float> target_doses_end,
			array<const_view<float>, 3> target_coordinates,
			const global_gamma_index_params<float>& params)
		{
			if (is_aligned_to<float32_VectorSize>(gamma_index_output) &&
				is_aligned_to<float32_VectorSize>(reference_doses) &&
				is_aligned_to<float32_VectorSize>(reference_coordinates[0]) &&
				is_aligned_to<float32_VectorSize>(reference_coordinates[1]) &&
				is_aligned_to<float32_VectorSize>(reference_coordinates[2]) &&
				is_aligned_to<float32_VectorSize>(target_doses) &&
				is_aligned_to<float32_VectorSize>(target_coordinates[0]) &&
				is_aligned_to<float32_VectorSize>(target_coordinates[1]) &&
				is_aligned_to<float32_VectorSize>(target_coordinates[2])
				)
			{
				return detail::gamma_index_minimize_pass<true, float32_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<3>());
			}
			else
			{
				return detail::gamma_index_minimize_pass<false, float32_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<3>());
			}
		}

		error_code gamma_index_finalize_pass(view<float> gamma_index_output, view<float> gamma_index_output_end)
		{
			if (is_aligned_to<float32_VectorSize>(gamma_index_output))
			{
				return detail::gamma_index_finalize_pass<true, float32_VectorSize>(gamma_index_output, gamma_index_output_end);
			}
			else
			{
				return detail::gamma_index_finalize_pass<false, float32_VectorSize>(gamma_index_output, gamma_index_output_end);
			}
		}

		// -------- float64 --------

		constexpr size_t float64_VectorSize = SIMDPP_FAST_FLOAT64_SIZE;

		error_code gamma_index_initialize_pass(view<double> gamma_index_output, view<double> gamma_index_output_end)
		{
			if (is_aligned_to<float64_VectorSize>(gamma_index_output))
			{
				return detail::gamma_index_initialize_pass<true, float64_VectorSize>(gamma_index_output, gamma_index_output_end);
			}
			else
			{
				return detail::gamma_index_initialize_pass<false, float64_VectorSize>(gamma_index_output, gamma_index_output_end);
			}
		}

		error_code gamma_index_minimize_pass(
			view<double> gamma_index_output, view<double> gamma_index_output_end,
			const_view<double> reference_doses,
			array<const_view<double>, 1> reference_coordinates,
			const_view<double> target_doses, const_view<double> target_doses_end,
			array<const_view<double>, 1> target_coordinates,
			const local_gamma_index_params<double>& params)
		{
			if (is_aligned_to<float64_VectorSize>(gamma_index_output) &&
				is_aligned_to<float64_VectorSize>(reference_doses) &&
				is_aligned_to<float64_VectorSize>(reference_coordinates[0]) &&
				is_aligned_to<float64_VectorSize>(target_doses) &&
				is_aligned_to<float64_VectorSize>(target_coordinates[0])
				)
			{
				return detail::gamma_index_minimize_pass<true, float64_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<1>());
			}
			else
			{
				return detail::gamma_index_minimize_pass<false, float64_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<1>());
			}
		}
		error_code gamma_index_minimize_pass(
			view<double> gamma_index_output, view<double> gamma_index_output_end,
			const_view<double> reference_doses,
			array<const_view<double>, 2> reference_coordinates,
			const_view<double> target_doses, const_view<double> target_doses_end,
			array<const_view<double>, 2> target_coordinates,
			const local_gamma_index_params<double>& params)
		{
			if (is_aligned_to<float64_VectorSize>(gamma_index_output) &&
				is_aligned_to<float64_VectorSize>(reference_doses) &&
				is_aligned_to<float64_VectorSize>(reference_coordinates[0]) &&
				is_aligned_to<float64_VectorSize>(reference_coordinates[1]) &&
				is_aligned_to<float64_VectorSize>(target_doses) &&
				is_aligned_to<float64_VectorSize>(target_coordinates[0]) &&
				is_aligned_to<float64_VectorSize>(target_coordinates[1])
				)
			{
				return detail::gamma_index_minimize_pass<true, float64_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<2>());
			}
			else
			{
				return detail::gamma_index_minimize_pass<false, float64_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<2>());
			}
		}
		error_code gamma_index_minimize_pass(
			view<double> gamma_index_output, view<double> gamma_index_output_end,
			const_view<double> reference_doses,
			array<const_view<double>, 3> reference_coordinates,
			const_view<double> target_doses, const_view<double> target_doses_end,
			array<const_view<double>, 3> target_coordinates,
			const local_gamma_index_params<double>& params)
		{
			if (is_aligned_to<float64_VectorSize>(gamma_index_output) &&
				is_aligned_to<float64_VectorSize>(reference_doses) &&
				is_aligned_to<float64_VectorSize>(reference_coordinates[0]) &&
				is_aligned_to<float64_VectorSize>(reference_coordinates[1]) &&
				is_aligned_to<float64_VectorSize>(reference_coordinates[2]) &&
				is_aligned_to<float64_VectorSize>(target_doses) &&
				is_aligned_to<float64_VectorSize>(target_coordinates[0]) &&
				is_aligned_to<float64_VectorSize>(target_coordinates[1]) &&
				is_aligned_to<float64_VectorSize>(target_coordinates[2])
				)
			{
				return detail::gamma_index_minimize_pass<true, float64_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<3>());
			}
			else
			{
				return detail::gamma_index_minimize_pass<false, float64_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<3>());
			}
		}
		error_code gamma_index_minimize_pass(
			view<double> gamma_index_output, view<double> gamma_index_output_end,
			const_view<double> reference_doses,
			array<const_view<double>, 1> reference_coordinates,
			const_view<double> target_doses, const_view<double> target_doses_end,
			array<const_view<double>, 1> target_coordinates,
			const global_gamma_index_params<double>& params)
		{
			if (is_aligned_to<float64_VectorSize>(gamma_index_output) &&
				is_aligned_to<float64_VectorSize>(reference_doses) &&
				is_aligned_to<float64_VectorSize>(reference_coordinates[0]) &&
				is_aligned_to<float64_VectorSize>(target_doses) &&
				is_aligned_to<float64_VectorSize>(target_coordinates[0])
				)
			{
				return detail::gamma_index_minimize_pass<true, float64_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<1>());
			}
			else
			{
				return detail::gamma_index_minimize_pass<false, float64_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<1>());
			}
		}
		error_code gamma_index_minimize_pass(
			view<double> gamma_index_output, view<double> gamma_index_output_end,
			const_view<double> reference_doses,
			array<const_view<double>, 2> reference_coordinates,
			const_view<double> target_doses, const_view<double> target_doses_end,
			array<const_view<double>, 2> target_coordinates,
			const global_gamma_index_params<double>& params)
		{
			if (is_aligned_to<float64_VectorSize>(gamma_index_output) &&
				is_aligned_to<float64_VectorSize>(reference_doses) &&
				is_aligned_to<float64_VectorSize>(reference_coordinates[0]) &&
				is_aligned_to<float64_VectorSize>(reference_coordinates[1]) &&
				is_aligned_to<float64_VectorSize>(target_doses) &&
				is_aligned_to<float64_VectorSize>(target_coordinates[0]) &&
				is_aligned_to<float64_VectorSize>(target_coordinates[1])
				)
			{
				return detail::gamma_index_minimize_pass<true, float64_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<2>());
			}
			else
			{
				return detail::gamma_index_minimize_pass<false, float64_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<2>());
			}
		}
		error_code gamma_index_minimize_pass(
			view<double> gamma_index_output, view<double> gamma_index_output_end,
			const_view<double> reference_doses,
			array<const_view<double>, 3> reference_coordinates,
			const_view<double> target_doses, const_view<double> target_doses_end,
			array<const_view<double>, 3> target_coordinates,
			const global_gamma_index_params<double>& params)
		{
			if (is_aligned_to<float64_VectorSize>(gamma_index_output) &&
				is_aligned_to<float64_VectorSize>(reference_doses) &&
				is_aligned_to<float64_VectorSize>(reference_coordinates[0]) &&
				is_aligned_to<float64_VectorSize>(reference_coordinates[1]) &&
				is_aligned_to<float64_VectorSize>(reference_coordinates[2]) &&
				is_aligned_to<float64_VectorSize>(target_doses) &&
				is_aligned_to<float64_VectorSize>(target_coordinates[0]) &&
				is_aligned_to<float64_VectorSize>(target_coordinates[1]) &&
				is_aligned_to<float64_VectorSize>(target_coordinates[2])
				)
			{
				return detail::gamma_index_minimize_pass<true, float64_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<3>());
			}
			else
			{
				return detail::gamma_index_minimize_pass<false, float64_VectorSize>(
					gamma_index_output, gamma_index_output_end,
					reference_doses,
					reference_coordinates,
					target_doses, target_doses_end,
					target_coordinates,
					params, std::make_index_sequence<3>());
			}
		}

		error_code gamma_index_finalize_pass(view<double> gamma_index_output, view<double> gamma_index_output_end)
		{
			if (is_aligned_to<float64_VectorSize>(gamma_index_output))
			{
				return detail::gamma_index_finalize_pass<true, float64_VectorSize>(gamma_index_output, gamma_index_output_end);
			}
			else
			{
				return detail::gamma_index_finalize_pass<false, float64_VectorSize>(gamma_index_output, gamma_index_output_end);
			}
		}
	}

	// -------- float32 --------

	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_initialize_pass)((view<float>)gamma_index_output, (view<float>)gamma_index_output_end));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_minimize_pass)
		((view<float>)gamma_index_output, (view<float>) gamma_index_output_end,
			(const_view<float>) reference_doses,
			(array<const_view<float>, 1>) reference_coordinates,
			(const_view<float>) target_doses, (const_view<float>) target_doses_end,
			(array<const_view<float>, 1>) target_coordinates,
			(const local_gamma_index_params<float>&) params));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_minimize_pass)
		((view<float>)gamma_index_output, (view<float>) gamma_index_output_end,
			(const_view<float>) reference_doses,
			(array<const_view<float>, 2>) reference_coordinates,
			(const_view<float>) target_doses, (const_view<float>) target_doses_end,
			(array<const_view<float>, 2>) target_coordinates,
			(const local_gamma_index_params<float>&) params));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_minimize_pass)
		((view<float>)gamma_index_output, (view<float>) gamma_index_output_end,
			(const_view<float>) reference_doses,
			(array<const_view<float>, 3>) reference_coordinates,
			(const_view<float>) target_doses, (const_view<float>) target_doses_end,
			(array<const_view<float>, 3>) target_coordinates,
			(const local_gamma_index_params<float>&) params));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_minimize_pass)
		((view<float>)gamma_index_output, (view<float>) gamma_index_output_end,
			(const_view<float>) reference_doses,
			(array<const_view<float>, 1>) reference_coordinates,
			(const_view<float>) target_doses, (const_view<float>) target_doses_end,
			(array<const_view<float>, 1>) target_coordinates,
			(const global_gamma_index_params<float>&) params));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_minimize_pass)
		((view<float>)gamma_index_output, (view<float>) gamma_index_output_end,
			(const_view<float>) reference_doses,
			(array<const_view<float>, 2>) reference_coordinates,
			(const_view<float>) target_doses, (const_view<float>) target_doses_end,
			(array<const_view<float>, 2>) target_coordinates,
			(const global_gamma_index_params<float>&) params));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_minimize_pass)
		((view<float>)gamma_index_output, (view<float>) gamma_index_output_end,
			(const_view<float>) reference_doses,
			(array<const_view<float>, 3>) reference_coordinates,
			(const_view<float>) target_doses, (const_view<float>) target_doses_end,
			(array<const_view<float>, 3>) target_coordinates,
			(const global_gamma_index_params<float>&) params));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_finalize_pass)((view<float>)gamma_index_output, (view<float>)gamma_index_output_end));

	// -------- float64 --------

	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_initialize_pass)((view<double>)gamma_index_output, (view<double>)gamma_index_output_end));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_minimize_pass)
		((view<double>)gamma_index_output, (view<double>) gamma_index_output_end,
			(const_view<double>) reference_doses,
			(array<const_view<double>, 1>) reference_coordinates,
			(const_view<double>) target_doses, (const_view<double>) target_doses_end,
			(array<const_view<double>, 1>) target_coordinates,
			(const local_gamma_index_params<double>&) params));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_minimize_pass)
		((view<double>)gamma_index_output, (view<double>) gamma_index_output_end,
			(const_view<double>) reference_doses,
			(array<const_view<double>, 2>) reference_coordinates,
			(const_view<double>) target_doses, (const_view<double>) target_doses_end,
			(array<const_view<double>, 2>) target_coordinates,
			(const local_gamma_index_params<double>&) params));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_minimize_pass)
		((view<double>)gamma_index_output, (view<double>) gamma_index_output_end,
			(const_view<double>) reference_doses,
			(array<const_view<double>, 3>) reference_coordinates,
			(const_view<double>) target_doses, (const_view<double>) target_doses_end,
			(array<const_view<double>, 3>) target_coordinates,
			(const local_gamma_index_params<double>&) params));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_minimize_pass)
		((view<double>)gamma_index_output, (view<double>) gamma_index_output_end,
			(const_view<double>) reference_doses,
			(array<const_view<double>, 1>) reference_coordinates,
			(const_view<double>) target_doses, (const_view<double>) target_doses_end,
			(array<const_view<double>, 1>) target_coordinates,
			(const global_gamma_index_params<double>&) params));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_minimize_pass)
		((view<double>)gamma_index_output, (view<double>) gamma_index_output_end,
			(const_view<double>) reference_doses,
			(array<const_view<double>, 2>) reference_coordinates,
			(const_view<double>) target_doses, (const_view<double>) target_doses_end,
			(array<const_view<double>, 2>) target_coordinates,
			(const global_gamma_index_params<double>&) params));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_minimize_pass)
		((view<double>)gamma_index_output, (view<double>) gamma_index_output_end,
			(const_view<double>) reference_doses,
			(array<const_view<double>, 3>) reference_coordinates,
			(const_view<double>) target_doses, (const_view<double>) target_doses_end,
			(array<const_view<double>, 3>) target_coordinates,
			(const global_gamma_index_params<double>&) params));
	SIMDPP_MAKE_DISPATCHER((error_code)(gamma_index_finalize_pass)((view<double>)gamma_index_output, (view<double>)gamma_index_output_end));
}