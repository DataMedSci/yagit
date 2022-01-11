#pragma once

#include <math/single/vectorized/common.hpp>

namespace yagit::core::math::vectorized::single
{
	template<typename ElementType, size_t VectorSize, LocalVectorGammaIndexParameters<ElementType, VectorSize> LocalGIParams>
	inline vec_t<ElementType, VectorSize> dose_difference(
		const vec_t<ElementType, VectorSize>& reference_dose, const vec_t<ElementType, VectorSize>& target_dose,
		const LocalGIParams& params)
	{
		auto intermediate = (reference_dose - target_dose) / (reference_dose * reference_dose_percentage<ElementType, VectorSize>(params));
		return intermediate * intermediate;
	}

	template<typename ElementType, size_t VectorSize, GlobalVectorGammaIndexParameters<ElementType, VectorSize> GlobalGIParams>
	inline vec_t<ElementType, VectorSize> dose_difference(
		const vec_t<ElementType, VectorSize>& reference_dose, const vec_t<ElementType, VectorSize>& target_dose,
		const GlobalGIParams& params)
	{
		auto intermediate = (reference_dose - target_dose);
		return (intermediate * intermediate) / absolute_dose_difference_squared<ElementType, VectorSize>(params);
	}

	template<typename ElementType, size_t VectorSize, GenericVectorGammaIndexParameters<ElementType, VectorSize> GenericGIParams>
	inline vec_t<ElementType, VectorSize> distance_to_agreement(
		const vec_t<ElementType, VectorSize>& reference_x,
		const vec_t<ElementType, VectorSize>& target_x,
		const GenericGIParams& params)
	{
		auto intermediate_x = (target_x - reference_x);
		return (intermediate_x * intermediate_x) / distance_to_agreement_normalization_squared<ElementType, VectorSize>(params);
	}

	template<typename ElementType, size_t VectorSize, GenericVectorGammaIndexParameters<ElementType, VectorSize> GenericGIParams>
	inline vec_t<ElementType, VectorSize> distance_to_agreement(
		const vec_t<ElementType, VectorSize>& reference_x, const vec_t<ElementType, VectorSize>& reference_y,
		const vec_t<ElementType, VectorSize>& target_x, const vec_t<ElementType, VectorSize>& target_y,
		const GenericGIParams& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y) / distance_to_agreement_normalization_squared<ElementType, VectorSize>(params);
	}

	template<typename ElementType, size_t VectorSize, GenericVectorGammaIndexParameters<ElementType, VectorSize> GenericGIParams>
	inline vec_t<ElementType, VectorSize> distance_to_agreement(
		const vec_t<ElementType, VectorSize>& reference_x, const vec_t<ElementType, VectorSize>& reference_y, const vec_t<ElementType, VectorSize>& reference_z,
		const vec_t<ElementType, VectorSize>& target_x, const vec_t<ElementType, VectorSize>& target_y, const vec_t<ElementType, VectorSize>& target_z,
		const GenericGIParams& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		auto intermediate_z = (target_z - reference_z);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y + intermediate_z * intermediate_z) / distance_to_agreement_normalization_squared<ElementType, VectorSize>(params);
	}

	template<typename ElementType, size_t VectorSize, LocalOrGlobalVectorGammaIndexParameters<ElementType, VectorSize> GIParams>
	inline vec_t<ElementType, VectorSize> gamma_index(
		const vec_t<ElementType, VectorSize>& reference_dose,
		const vec_t<ElementType, VectorSize>& target_dose,
		const vec_t<ElementType, VectorSize>& reference_x,
		const vec_t<ElementType, VectorSize>& target_x,
		const GIParams& params)
	{
		return dose_difference<ElementType, VectorSize>(reference_dose, target_dose, params) + distance_to_agreement<ElementType, VectorSize>(reference_x, target_x, params);
	}

	template<typename ElementType, size_t VectorSize, LocalOrGlobalVectorGammaIndexParameters<ElementType, VectorSize> GIParams>
	inline vec_t<ElementType, VectorSize> gamma_index(
		const vec_t<ElementType, VectorSize>& reference_dose,
		const vec_t<ElementType, VectorSize>& target_dose,
		const vec_t<ElementType, VectorSize>& reference_x, const vec_t<ElementType, VectorSize>& reference_y,
		const vec_t<ElementType, VectorSize>& target_x, const vec_t<ElementType, VectorSize>& target_y,
		const GIParams& params)
	{
		return dose_difference<ElementType, VectorSize>(reference_dose, target_dose, params) + distance_to_agreement<ElementType, VectorSize>(reference_x, reference_y, target_x, target_y, params);
	}

	template<typename ElementType, size_t VectorSize, LocalOrGlobalVectorGammaIndexParameters<ElementType, VectorSize> GIParams>
	inline vec_t<ElementType, VectorSize> gamma_index(
		const vec_t<ElementType, VectorSize>& reference_dose,
		const vec_t<ElementType, VectorSize>& target_dose,
		const vec_t<ElementType, VectorSize>& reference_x, const vec_t<ElementType, VectorSize>& reference_y, const vec_t<ElementType, VectorSize>& reference_z,
		const vec_t<ElementType, VectorSize>& target_x, const vec_t<ElementType, VectorSize>& target_y, const vec_t<ElementType, VectorSize>& target_z,
		const GIParams& params)
	{
		return dose_difference<ElementType, VectorSize>(reference_dose, target_dose, params) + distance_to_agreement<ElementType, VectorSize>(reference_x, reference_y, reference_z, target_x, target_y, target_z, params);
	}
}