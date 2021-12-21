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
}