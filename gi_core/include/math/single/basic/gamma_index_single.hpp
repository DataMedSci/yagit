#pragma once

#include <math/common.hpp>

namespace yagit::core::math::single
{
	template<typename ElementType, LocalGammaIndexParameters<ElementType> LocalGIParams>
	inline constexpr ElementType dose_difference(
		ElementType reference_dose, ElementType target_dose,
		const LocalGIParams& params)
	{
		auto intermediate = (reference_dose - target_dose) / (reference_dose * reference_dose_percentage<ElementType>(params));
		return intermediate * intermediate;
	}

	template<typename ElementType, GlobalGammaIndexParameters<ElementType> GlobalGIParams>
	inline constexpr ElementType dose_difference(
		ElementType reference_dose, ElementType target_dose,
		const GlobalGIParams& params)
	{
		auto intermediate = (reference_dose - target_dose);
		return (intermediate * intermediate) / absolute_dose_difference_squared<ElementType>(params);
	}

	template<typename ElementType, GenericGammaIndexParameters<ElementType> GenericGIParams>
	inline constexpr ElementType distance_to_agreement(
		ElementType reference_x,
		ElementType target_x,
		const GenericGIParams& params)
	{
		auto intermediate_x = (target_x - reference_x);
		return (intermediate_x * intermediate_x) / distance_to_agreement_normalization_squared<ElementType>(params);
	}

	template<typename ElementType, GenericGammaIndexParameters<ElementType> GenericGIParams>
	inline constexpr ElementType distance_to_agreement(
		ElementType reference_x, ElementType reference_y,
		ElementType target_x, ElementType target_y,
		const GenericGIParams& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y) / distance_to_agreement_normalization_squared<ElementType>(params);
	}

	template<typename ElementType, GenericGammaIndexParameters<ElementType> GenericGIParams>
	inline constexpr ElementType distance_to_agreement(
		ElementType reference_x, ElementType reference_y, ElementType reference_z,
		ElementType target_x, ElementType target_y, ElementType target_z,
		const GenericGIParams& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		auto intermediate_z = (target_z - reference_z);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y + intermediate_z * intermediate_z) / distance_to_agreement_normalization_squared<ElementType>(params);
	}

	template<typename ElementType, LocalOrGlobalGammaIndexParameters<ElementType> GIParams>
	inline constexpr ElementType gamma_index(
		ElementType reference_dose,
		ElementType target_dose,
		ElementType reference_x,
		ElementType target_x,
		const GIParams& params)
	{
		return dose_difference<ElementType>(reference_dose, target_dose, params) + distance_to_agreement<ElementType>(reference_x, target_x, params);
	}

	template<typename ElementType, LocalOrGlobalGammaIndexParameters<ElementType> GIParams>
	inline constexpr ElementType gamma_index(
		ElementType reference_dose,
		ElementType target_dose,
		ElementType reference_x, ElementType reference_y,
		ElementType target_x, ElementType target_y,
		const GIParams& params)
	{
		return dose_difference<ElementType>(reference_dose, target_dose, params) + distance_to_agreement<ElementType>(reference_x, reference_y, target_x, target_y, params);
	}

	template<typename ElementType, LocalOrGlobalGammaIndexParameters<ElementType> GIParams>
	inline constexpr ElementType gamma_index(
		ElementType reference_dose,
		ElementType target_dose,
		ElementType reference_x, ElementType reference_y, ElementType reference_z,
		ElementType target_x, ElementType target_y, ElementType target_z,
		const GIParams& params)
	{
		return dose_difference<ElementType>(reference_dose, target_dose, params) + distance_to_agreement<ElementType>(reference_x, reference_y, reference_z, target_x, target_y, target_z, params);
	}
}