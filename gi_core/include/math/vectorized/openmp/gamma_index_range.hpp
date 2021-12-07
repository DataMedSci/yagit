#pragma once

#include <math/vectorized/common.hpp>

#include <math/basic/openmp/gamma_index_range.hpp>

namespace yagit::core::math
{
	template<typename Type, size_t Dimensions>
	struct parallel_unsequenced_gamma_index_implementer : public parallel_gamma_index_implementer<Type, Dimensions> {};

	// -------- float32 --------

	template<>
	struct parallel_unsequenced_gamma_index_implementer<float, 1>
	{
		using value_type = float;

		static error_code initialize_pass(view<value_type> output, view<value_type> output_end);
		static error_code minimize_pass(
			view<value_type> output, view<value_type> output_end,
			const_view<value_type> reference_doses,
			array<const_view<value_type>, 1> reference_coordinates,
			const_view<value_type> target_doses, const_view<value_type> target_doses_end,
			array<const_view<value_type>, 1> target_coordinates,
			const local_gamma_index_params<value_type>& params);
		static error_code minimize_pass(
			view<value_type> output, view<value_type> output_end,
			const_view<value_type> reference_doses,
			array<const_view<value_type>, 1> reference_coordinates,
			const_view<value_type> target_doses, const_view<value_type> target_doses_end,
			array<const_view<value_type>, 1> target_coordinates,
			const global_gamma_index_params<value_type>& params);
		static error_code finalize_pass(view<value_type> output, view<value_type> output_end);
	};

	template<>
	struct parallel_unsequenced_gamma_index_implementer<float, 2>
	{
		using value_type = float;

		static error_code initialize_pass(view<value_type> output, view<value_type> output_end);
		static error_code minimize_pass(
			view<value_type> output, view<value_type> output_end,
			const_view<value_type> reference_doses,
			array<const_view<value_type>, 2> reference_coordinates,
			const_view<value_type> target_doses, const_view<value_type> target_doses_end,
			array<const_view<value_type>, 2> target_coordinates,
			const local_gamma_index_params<value_type>& params);
		static error_code minimize_pass(
			view<value_type> output, view<value_type> output_end,
			const_view<value_type> reference_doses,
			array<const_view<value_type>, 2> reference_coordinates,
			const_view<value_type> target_doses, const_view<value_type> target_doses_end,
			array<const_view<value_type>, 2> target_coordinates,
			const global_gamma_index_params<value_type>& params);
		static error_code finalize_pass(view<value_type> output, view<value_type> output_end);
	};

	template<>
	struct parallel_unsequenced_gamma_index_implementer<float, 3>
	{
		using value_type = float;

		static error_code initialize_pass(view<value_type> output, view<value_type> output_end);
		static error_code minimize_pass(
			view<value_type> output, view<value_type> output_end,
			const_view<value_type> reference_doses,
			array<const_view<value_type>, 3> reference_coordinates,
			const_view<value_type> target_doses, const_view<value_type> target_doses_end,
			array<const_view<value_type>, 3> target_coordinates,
			const local_gamma_index_params<value_type>& params);
		static error_code minimize_pass(
			view<value_type> output, view<value_type> output_end,
			const_view<value_type> reference_doses,
			array<const_view<value_type>, 3> reference_coordinates,
			const_view<value_type> target_doses, const_view<value_type> target_doses_end,
			array<const_view<value_type>, 3> target_coordinates,
			const global_gamma_index_params<value_type>& params);
		static error_code finalize_pass(view<value_type> output, view<value_type> output_end);
	};

	// -------- float64 --------

	template<>
	struct parallel_unsequenced_gamma_index_implementer<double, 1>
	{
		using value_type = double;

		static error_code initialize_pass(view<value_type> output, view<value_type> output_end);
		static error_code minimize_pass(
			view<value_type> output, view<value_type> output_end,
			const_view<value_type> reference_doses,
			array<const_view<value_type>, 1> reference_coordinates,
			const_view<value_type> target_doses, const_view<value_type> target_doses_end,
			array<const_view<value_type>, 1> target_coordinates,
			const local_gamma_index_params<value_type>& params);
		static error_code minimize_pass(
			view<value_type> output, view<value_type> output_end,
			const_view<value_type> reference_doses,
			array<const_view<value_type>, 1> reference_coordinates,
			const_view<value_type> target_doses, const_view<value_type> target_doses_end,
			array<const_view<value_type>, 1> target_coordinates,
			const global_gamma_index_params<value_type>& params);
		static error_code finalize_pass(view<value_type> output, view<value_type> output_end);
	};

	template<>
	struct parallel_unsequenced_gamma_index_implementer<double, 2>
	{
		using value_type = double;

		static error_code initialize_pass(view<value_type> output, view<value_type> output_end);
		static error_code minimize_pass(
			view<value_type> output, view<value_type> output_end,
			const_view<value_type> reference_doses,
			array<const_view<value_type>, 2> reference_coordinates,
			const_view<value_type> target_doses, const_view<value_type> target_doses_end,
			array<const_view<value_type>, 2> target_coordinates,
			const local_gamma_index_params<value_type>& params);
		static error_code minimize_pass(
			view<value_type> output, view<value_type> output_end,
			const_view<value_type> reference_doses,
			array<const_view<value_type>, 2> reference_coordinates,
			const_view<value_type> target_doses, const_view<value_type> target_doses_end,
			array<const_view<value_type>, 2> target_coordinates,
			const global_gamma_index_params<value_type>& params);
		static error_code finalize_pass(view<value_type> output, view<value_type> output_end);
	};

	template<>
	struct parallel_unsequenced_gamma_index_implementer<double, 3>
	{
		using value_type = double;

		static error_code initialize_pass(view<value_type> output, view<value_type> output_end);
		static error_code minimize_pass(
			view<value_type> output, view<value_type> output_end,
			const_view<value_type> reference_doses,
			array<const_view<value_type>, 3> reference_coordinates,
			const_view<value_type> target_doses, const_view<value_type> target_doses_end,
			array<const_view<value_type>, 3> target_coordinates,
			const local_gamma_index_params<value_type>& params);
		static error_code minimize_pass(
			view<value_type> output, view<value_type> output_end,
			const_view<value_type> reference_doses,
			array<const_view<value_type>, 3> reference_coordinates,
			const_view<value_type> target_doses, const_view<value_type> target_doses_end,
			array<const_view<value_type>, 3> target_coordinates,
			const global_gamma_index_params<value_type>& params);
		static error_code finalize_pass(view<value_type> output, view<value_type> output_end);
	};
}