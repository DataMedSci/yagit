#pragma once


#include <math/basic/gamma_index_single.hpp>
#include <execution>
#if !defined(YAGIT_OPENMP)
#include <math/basic/gamma_index_range_impl.hpp>
#endif

namespace yagit::core::math::basic::openmp
{
	namespace detail
	{
#if defined(YAGIT_OPENMP)
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
			size_t output_size = gamma_index_output_end - gamma_index_output;
#pragma omp parallel for
			for (long i = 0; i < output_size; i++)
			{
				auto gi_output = gamma_index_output + i;
				auto ref_doses = reference_doses + i;
				array<const_view<Type>, Dimensions> ref_coordinates = { (reference_coordinates[I] + i)... };

				if (std::isnan(*gi_output) || *gi_output < static_cast<Type>(1))
					continue;

				if (std::isnan(*ref_doses))
				{
					*gi_output = *ref_doses;
					continue;
				}

				Type min_gi = numeric_limits<Type>::max();
				const_view<Type> t_doses = target_doses;
				array<const_view<Type>, Dimensions> t_coordinates = target_coordinates;
				for (;
					t_doses != target_doses_end;
					++t_doses, ((++t_coordinates[I]),...))
				{
					if (std::isnan(*t_doses))
						continue;

					min_gi = std::min(
						min_gi,
						gamma_index(*ref_doses, *t_doses, (*ref_coordinates[I])..., (*t_coordinates[I])..., params)
					);

					if (min_gi < static_cast<Type>(1))
						break;
				}

				*gi_output = std::min(*gi_output, min_gi);
			}

			return {};
		}
#else
		using basic::detail::gamma_index_minimize_pass;
#endif
	}

	template<typename Type>
	constexpr error_code gamma_index_initialize_pass(view<Type> gamma_index_output, view<Type> gamma_index_output_end)
	{
		std::fill(std::execution::par, gamma_index_output, gamma_index_output_end, std::numeric_limits<Type>::max());
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
		std::transform(std::execution::par, gamma_index_output, gamma_index_output_end, gamma_index_output, [](auto v) {return std::sqrt(v); });
		return {};
	}
}