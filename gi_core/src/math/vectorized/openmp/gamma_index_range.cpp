#include <math/vectorized/openmp/gamma_index_range.hpp>
#include <math/vectorized/openmp/gamma_index_range_impl.hpp>

namespace yagit::core::math
{
	// -------- float32 --------

	error_code parallel_unsequenced_gamma_index_implementer<float, 1>::initialize_pass(
		view<value_type> output, view<value_type> output_end)
	{
		return vectorized::openmp::detail::gamma_index_initialize_pass(output, output_end);
	}

	error_code parallel_unsequenced_gamma_index_implementer<float, 1>::minimize_pass(
		view<value_type> output, view<value_type> output_end,
		const_view<value_type> reference_doses,
		array<const_view<value_type>, 1> reference_coordinates,
		const_view<value_type> target_doses, const_view<value_type> target_doses_end,
		array<const_view<value_type>, 1> target_coordinates,
		const local_gamma_index_params<value_type>& params)
	{
		return vectorized::openmp::detail::gamma_index_minimize_pass(
			output, output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params);
	}

	error_code parallel_unsequenced_gamma_index_implementer<float, 1>::minimize_pass(
		view<value_type> output, view<value_type> output_end,
		const_view<value_type> reference_doses,
		array<const_view<value_type>, 1> reference_coordinates,
		const_view<value_type> target_doses, const_view<value_type> target_doses_end,
		array<const_view<value_type>, 1> target_coordinates,
		const global_gamma_index_params<value_type>& params)
	{
		return vectorized::openmp::detail::gamma_index_minimize_pass(
			output, output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params);
	}

	error_code parallel_unsequenced_gamma_index_implementer<float, 1>::finalize_pass(
		view<value_type> output, view<value_type> output_end)
	{
		return vectorized::openmp::detail::gamma_index_finalize_pass(output, output_end);
	}

	error_code parallel_unsequenced_gamma_index_implementer<float, 2>::initialize_pass(
		view<value_type> output, view<value_type> output_end)
	{
		return vectorized::openmp::detail::gamma_index_initialize_pass(output, output_end);
	}

	error_code parallel_unsequenced_gamma_index_implementer<float, 2>::minimize_pass(
		view<value_type> output, view<value_type> output_end,
		const_view<value_type> reference_doses,
		array<const_view<value_type>, 2> reference_coordinates,
		const_view<value_type> target_doses, const_view<value_type> target_doses_end,
		array<const_view<value_type>, 2> target_coordinates,
		const local_gamma_index_params<value_type>& params)
	{
		return vectorized::openmp::detail::gamma_index_minimize_pass(
			output, output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params);
	}

	error_code parallel_unsequenced_gamma_index_implementer<float, 2>::minimize_pass(
		view<value_type> output, view<value_type> output_end,
		const_view<value_type> reference_doses,
		array<const_view<value_type>, 2> reference_coordinates,
		const_view<value_type> target_doses, const_view<value_type> target_doses_end,
		array<const_view<value_type>, 2> target_coordinates,
		const global_gamma_index_params<value_type>& params)
	{
		return vectorized::openmp::detail::gamma_index_minimize_pass(
			output, output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params);
	}

	error_code parallel_unsequenced_gamma_index_implementer<float, 2>::finalize_pass(
		view<value_type> output, view<value_type> output_end)
	{
		return vectorized::openmp::detail::gamma_index_finalize_pass(output, output_end);
	}

	error_code parallel_unsequenced_gamma_index_implementer<float, 3>::initialize_pass(
		view<value_type> output, view<value_type> output_end)
	{
		return vectorized::openmp::detail::gamma_index_initialize_pass(output, output_end);
	}

	error_code parallel_unsequenced_gamma_index_implementer<float, 3>::minimize_pass(
		view<value_type> output, view<value_type> output_end,
		const_view<value_type> reference_doses,
		array<const_view<value_type>, 3> reference_coordinates,
		const_view<value_type> target_doses, const_view<value_type> target_doses_end,
		array<const_view<value_type>, 3> target_coordinates,
		const local_gamma_index_params<value_type>& params)
	{
		return vectorized::openmp::detail::gamma_index_minimize_pass(
			output, output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params);
	}

	error_code parallel_unsequenced_gamma_index_implementer<float, 3>::minimize_pass(
		view<value_type> output, view<value_type> output_end,
		const_view<value_type> reference_doses,
		array<const_view<value_type>, 3> reference_coordinates,
		const_view<value_type> target_doses, const_view<value_type> target_doses_end,
		array<const_view<value_type>, 3> target_coordinates,
		const global_gamma_index_params<value_type>& params)
	{
		return vectorized::openmp::detail::gamma_index_minimize_pass(
			output, output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params);
	}

	error_code parallel_unsequenced_gamma_index_implementer<float, 3>::finalize_pass(
		view<value_type> output, view<value_type> output_end)
	{
		return vectorized::openmp::detail::gamma_index_finalize_pass(output, output_end);
	}

	// -------- float64 --------

	error_code parallel_unsequenced_gamma_index_implementer<double, 1>::initialize_pass(
		view<value_type> output, view<value_type> output_end)
	{
		return vectorized::openmp::detail::gamma_index_initialize_pass(output, output_end);
	}

	error_code parallel_unsequenced_gamma_index_implementer<double, 1>::minimize_pass(
		view<value_type> output, view<value_type> output_end,
		const_view<value_type> reference_doses,
		array<const_view<value_type>, 1> reference_coordinates,
		const_view<value_type> target_doses, const_view<value_type> target_doses_end,
		array<const_view<value_type>, 1> target_coordinates,
		const local_gamma_index_params<value_type>& params)
	{
		return vectorized::openmp::detail::gamma_index_minimize_pass(
			output, output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params);
	}

	error_code parallel_unsequenced_gamma_index_implementer<double, 1>::minimize_pass(
		view<value_type> output, view<value_type> output_end,
		const_view<value_type> reference_doses,
		array<const_view<value_type>, 1> reference_coordinates,
		const_view<value_type> target_doses, const_view<value_type> target_doses_end,
		array<const_view<value_type>, 1> target_coordinates,
		const global_gamma_index_params<value_type>& params)
	{
		return vectorized::openmp::detail::gamma_index_minimize_pass(
			output, output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params);
	}

	error_code parallel_unsequenced_gamma_index_implementer<double, 1>::finalize_pass(
		view<value_type> output, view<value_type> output_end)
	{
		return vectorized::openmp::detail::gamma_index_finalize_pass(output, output_end);
	}

	error_code parallel_unsequenced_gamma_index_implementer<double, 2>::initialize_pass(
		view<value_type> output, view<value_type> output_end)
	{
		return vectorized::openmp::detail::gamma_index_initialize_pass(output, output_end);
	}

	error_code parallel_unsequenced_gamma_index_implementer<double, 2>::minimize_pass(
		view<value_type> output, view<value_type> output_end,
		const_view<value_type> reference_doses,
		array<const_view<value_type>, 2> reference_coordinates,
		const_view<value_type> target_doses, const_view<value_type> target_doses_end,
		array<const_view<value_type>, 2> target_coordinates,
		const local_gamma_index_params<value_type>& params)
	{
		return vectorized::openmp::detail::gamma_index_minimize_pass(
			output, output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params);
	}

	error_code parallel_unsequenced_gamma_index_implementer<double, 2>::minimize_pass(
		view<value_type> output, view<value_type> output_end,
		const_view<value_type> reference_doses,
		array<const_view<value_type>, 2> reference_coordinates,
		const_view<value_type> target_doses, const_view<value_type> target_doses_end,
		array<const_view<value_type>, 2> target_coordinates,
		const global_gamma_index_params<value_type>& params)
	{
		return vectorized::openmp::detail::gamma_index_minimize_pass(
			output, output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params);
	}

	error_code parallel_unsequenced_gamma_index_implementer<double, 2>::finalize_pass(
		view<value_type> output, view<value_type> output_end)
	{
		return vectorized::openmp::detail::gamma_index_finalize_pass(output, output_end);
	}

	error_code parallel_unsequenced_gamma_index_implementer<double, 3>::initialize_pass(
		view<value_type> output, view<value_type> output_end)
	{
		return vectorized::openmp::detail::gamma_index_initialize_pass(output, output_end);
	}

	error_code parallel_unsequenced_gamma_index_implementer<double, 3>::minimize_pass(
		view<value_type> output, view<value_type> output_end,
		const_view<value_type> reference_doses,
		array<const_view<value_type>, 3> reference_coordinates,
		const_view<value_type> target_doses, const_view<value_type> target_doses_end,
		array<const_view<value_type>, 3> target_coordinates,
		const local_gamma_index_params<value_type>& params)
	{
		return vectorized::openmp::detail::gamma_index_minimize_pass(
			output, output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params);
	}

	error_code parallel_unsequenced_gamma_index_implementer<double, 3>::minimize_pass(
		view<value_type> output, view<value_type> output_end,
		const_view<value_type> reference_doses,
		array<const_view<value_type>, 3> reference_coordinates,
		const_view<value_type> target_doses, const_view<value_type> target_doses_end,
		array<const_view<value_type>, 3> target_coordinates,
		const global_gamma_index_params<value_type>& params)
	{
		return vectorized::openmp::detail::gamma_index_minimize_pass(
			output, output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params);
	}

	error_code parallel_unsequenced_gamma_index_implementer<double, 3>::finalize_pass(
		view<value_type> output, view<value_type> output_end)
	{
		return vectorized::openmp::detail::gamma_index_finalize_pass(output, output_end);
	}
}