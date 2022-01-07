#pragma once

#include <common.hpp>
#include <cmath>

namespace yagit::core::math
{
	template<typename T>
	struct local_gamma_index_params
	{
		using value_type = T;

		value_type percentage;
		value_type distance_to_agreement_squared;
	};

	template<typename T>
	struct global_gamma_index_params
	{
		using value_type = T;

		value_type dose_difference_squared;
		value_type distance_to_agreement_squared;
	};

	template<typename T>
	struct gamma_index_params
	{
		using value_type = T;
		using variant_type = variant<local_gamma_index_params<value_type>, global_gamma_index_params<value_type>>;

		variant_type params;
	};

	namespace algorithm_version
	{
		namespace old
		{
			struct classic {};
			struct classic_with_rectangle {};
			struct spiral {};
			struct spiral_with_rectangle {};
		}
		struct classic {};
	}

	namespace execution
	{
		using namespace std::execution;
	}

	namespace helpers
	{
		template<typename Type, size_t Dimensions, typename ParamsType, typename AlgorithmVersion, typename Execution>
		struct gi_params
		{
			view<Type> gamma_index_output;
			view<Type> gamma_index_output_end;
			const_view<Type> reference_doses;
			array<const_view<Type>, Dimensions> reference_coordinates;
			const_view<Type> target_doses;
			const_view<Type> target_doses_end;
			array<const_view<Type>, Dimensions> target_coordinates;
			const ParamsType& params;
			const AlgorithmVersion& algorithm_version;
			const Execution& execution;
		};
	}
}