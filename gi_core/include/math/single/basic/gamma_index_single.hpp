#pragma once

#include <math/common.hpp>

namespace yagit::core::math::single::basic
{
	template<typename Type>
	inline constexpr Type dose_difference(
		Type reference_dose, Type target_dose,
		const local_gamma_index_params<Type>& params)
	{
		auto intermediate = (reference_dose - target_dose) / (reference_dose * params.percentage);
		return intermediate * intermediate;
	}

	template<typename Type>
	inline constexpr Type dose_difference(
		Type reference_dose, Type target_dose,
		const global_gamma_index_params<Type>& params)
	{
		auto intermediate = (reference_dose - target_dose);
		return (intermediate * intermediate) / params.dose_difference_squared;
	}

	template<typename Type>
	inline constexpr Type distance_to_agreement(
		Type reference_x,
		Type target_x,
		const local_gamma_index_params<Type>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		return (intermediate_x * intermediate_x) / params.distance_to_agreement_squared;
	}

	template<typename Type>
	inline constexpr Type distance_to_agreement(
		Type reference_x, Type reference_y,
		Type target_x, Type target_y,
		const local_gamma_index_params<Type>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y) / params.distance_to_agreement_squared;
	}

	template<typename Type>
	inline constexpr Type distance_to_agreement(
		Type reference_x, Type reference_y, Type reference_z,
		Type target_x, Type target_y, Type target_z,
		const local_gamma_index_params<Type>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		auto intermediate_z = (target_z - reference_z);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y + intermediate_z * intermediate_z) / params.distance_to_agreement_squared;
	}

	template<typename Type>
	inline constexpr Type distance_to_agreement(
		Type reference_x,
		Type target_x,
		const global_gamma_index_params<Type>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		return (intermediate_x * intermediate_x) / params.distance_to_agreement_squared;
	}

	template<typename Type>
	inline constexpr Type distance_to_agreement(
		Type reference_x, Type reference_y,
		Type target_x, Type target_y,
		const global_gamma_index_params<Type>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y) / params.distance_to_agreement_squared;
	}

	template<typename Type>
	inline constexpr Type distance_to_agreement(
		Type reference_x, Type reference_y, Type reference_z,
		Type target_x, Type target_y, Type target_z,
		const global_gamma_index_params<Type>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		auto intermediate_z = (target_z - reference_z);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y + intermediate_z * intermediate_z) / params.distance_to_agreement_squared;
	}

	template<typename Type>
	inline constexpr Type gamma_index(
		Type reference_dose,
		Type target_dose,
		Type reference_x,
		Type target_x,
		const local_gamma_index_params<Type>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, target_x, params);
	}

	template<typename Type>
	inline constexpr Type gamma_index(
		Type reference_dose,
		Type target_dose,
		Type reference_x, Type reference_y,
		Type target_x, Type target_y,
		const local_gamma_index_params<Type>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, reference_y, target_x, target_y, params);
	}

	template<typename Type>
	inline constexpr Type gamma_index(
		Type reference_dose, 
		Type target_dose,
		Type reference_x, Type reference_y, Type reference_z,
		Type target_x, Type target_y, Type target_z,
		const local_gamma_index_params<Type>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, reference_y, reference_z, target_x, target_y, target_z, params);
	}

	template<typename Type>
	inline constexpr Type gamma_index(
		Type reference_dose,
		Type target_dose,
		Type reference_x,
		Type target_x,
		const global_gamma_index_params<Type>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, target_x, params);
	}

	template<typename Type>
	inline constexpr Type gamma_index(
		Type reference_dose,
		Type target_dose,
		Type reference_x, Type reference_y,
		Type target_x, Type target_y,
		const global_gamma_index_params<Type>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, reference_y, target_x, target_y, params);
	}

	template<typename Type>
	inline constexpr Type gamma_index(
		Type reference_dose,
		Type target_dose,
		Type reference_x, Type reference_y, Type reference_z,
		Type target_x, Type target_y, Type target_z,
		const global_gamma_index_params<Type>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, reference_y, reference_z, target_x, target_y, target_z, params);
	}
}