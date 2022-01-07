#pragma once

#include <math/single/basic/gamma_index_single.hpp>

namespace yagit::core::math::classic::basic::detail
{
	using single::basic::gamma_index;

	template<typename Type, size_t Dimensions, typename ParamsType, size_t... I>
	constexpr error_code gamma_index_minimize_pass(
		view<Type> gamma_index_output, view<Type> gamma_index_output_end,
		const_view<Type> reference_doses,
		array<const_view<Type>, Dimensions> reference_coordinates,
		const_view<Type> target_doses, const_view<Type> target_doses_end,
		array<const_view<Type>, Dimensions> target_coordinates,
		const ParamsType& params,
		index_sequence<I...>)
	{
		for (;
			gamma_index_output != gamma_index_output_end;
			++gamma_index_output, ++reference_doses, ((++reference_coordinates[I]), ...))
		{
			if (std::isnan(*reference_doses))
			{
				*gamma_index_output = *reference_doses;
				continue;
			}

			Type min_gi = numeric_limits<Type>::max();
			const_view<Type> t_doses = target_doses;
			array<const_view<Type>, Dimensions> t_coordinates = target_coordinates;
			for (;
				t_doses != target_doses_end;
				++t_doses, ((++t_coordinates[I]), ...))
			{
				if (std::isnan(*t_doses))
					continue;

				min_gi = std::min(
					min_gi,
					gamma_index(*reference_doses, *t_doses, (*reference_coordinates[I])..., (*t_coordinates[I])..., params)
				);

				if (min_gi <= static_cast<Type>(1))
					break;
			}

			*gamma_index_output = std::min(*gamma_index_output, min_gi);
		}

		return {};
	}

	template<typename Type>
	constexpr error_code gamma_index_initialize_pass(view<Type> gamma_index_output, view<Type> gamma_index_output_end)
	{
		std::fill(gamma_index_output, gamma_index_output_end, std::numeric_limits<Type>::max());
		return {};
	}

	template<typename Type, size_t Dimensions, typename ParamsType>
	constexpr error_code gamma_index_minimize_pass(
		view<Type> gamma_index_output, view<Type> gamma_index_output_end,
		const_view<Type> reference_doses,
		array<const_view<Type>, Dimensions> reference_coordinates,
		const_view<Type> target_doses, const_view<Type> target_doses_end,
		array<const_view<Type>, Dimensions> target_coordinates,
		const ParamsType& params)
	{
		return detail::gamma_index_minimize_pass(
			gamma_index_output, gamma_index_output_end,
			reference_doses,
			reference_coordinates,
			target_doses, target_doses_end,
			target_coordinates,
			params,
			std::make_index_sequence<Dimensions>()
		);
	}

	template<typename Type>
	constexpr error_code gamma_index_finalize_pass(view<Type> gamma_index_output, view<Type> gamma_index_output_end)
	{
		std::transform(gamma_index_output, gamma_index_output_end, gamma_index_output, [](auto v) {return std::sqrt(v); });
		return {};
	}
}