#pragma once

#include <math/basic/gamma_index_range_impl.hpp>

namespace yagit::core::math
{
	template<typename Type, size_t Dimensions>
	struct sequenced_gamma_index_implementer {};

	template<typename Type>
	struct sequenced_gamma_index_implementer<Type, 1>
	{
		constexpr static error_code initialize_pass(view<Type> output, view<Type> output_end)
		{
			return basic::gamma_index_initialize_pass(output, output_end);
		}
		constexpr static error_code minimize_pass(
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			std::array<const_view<Type>, 1> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			std::array<const_view<Type>, 1> target_coordinates,
			const local_gamma_index_params<Type>& params)
		{
			return basic::gamma_index_minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}
		constexpr static error_code minimize_pass(
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			std::array<const_view<Type>, 1> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			std::array<const_view<Type>, 1> target_coordinates,
			const global_gamma_index_params<Type>& params)
		{
			return basic::gamma_index_minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}
		constexpr static error_code finalize_pass(view<Type> output, view<Type> output_end)
		{
			return basic::gamma_index_finalize_pass(output, output_end);
		}
	};

	template<typename Type>
	struct sequenced_gamma_index_implementer<Type, 2>
	{
		constexpr static error_code initialize_pass(view<Type> output, view<Type> output_end)
		{
			return basic::gamma_index_initialize_pass(output, output_end);
		}
		constexpr static error_code minimize_pass(
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			std::array<const_view<Type>, 2> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			std::array<const_view<Type>, 2> target_coordinates,
			const local_gamma_index_params<Type>& params)
		{
			return basic::gamma_index_minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}
		constexpr static error_code minimize_pass(
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			std::array<const_view<Type>, 2> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			std::array<const_view<Type>, 2> target_coordinates,
			const global_gamma_index_params<Type>& params)
		{
			return basic::gamma_index_minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}
		constexpr static error_code finalize_pass(view<Type> output, view<Type> output_end)
		{
			return basic::gamma_index_finalize_pass(output, output_end);
		}
	};

	template<typename Type>
	struct sequenced_gamma_index_implementer<Type, 3>
	{
		constexpr static error_code initialize_pass(view<Type> output, view<Type> output_end)
		{
			return basic::gamma_index_initialize_pass(output, output_end);
		}
		constexpr static error_code minimize_pass(
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			std::array<const_view<Type>, 3> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			std::array<const_view<Type>, 3> target_coordinates,
			const local_gamma_index_params<Type>& params)
		{
			return basic::gamma_index_minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}
		constexpr static error_code minimize_pass(
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			std::array<const_view<Type>, 3> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			std::array<const_view<Type>, 3> target_coordinates,
			const global_gamma_index_params<Type>& params)
		{
			return basic::gamma_index_minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}
		constexpr static error_code finalize_pass(view<Type> output, view<Type> output_end)
		{
			return basic::gamma_index_finalize_pass(output, output_end);
		}
	};
}