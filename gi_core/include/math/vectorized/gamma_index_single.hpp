#pragma once

#include <math/vectorized/common.hpp>

namespace yagit::core::math::vectorized
{
	// -------- DD float32 --------

	template<unsigned VectorSize, typename E0, typename E1>
	inline simdpp::float32<VectorSize> dose_difference(
		const simdpp::float32<VectorSize, E0>& reference_dose, const simdpp::float32<VectorSize, E1>& target_dose,
		const vlocal_gamma_index_params<float, VectorSize>& params)
	{
		auto intermediate = (reference_dose - target_dose) / (reference_dose * params.percentage);
		return intermediate * intermediate;
	}

	template<unsigned VectorSize, typename E0, typename E1>
	inline simdpp::float32<VectorSize> dose_difference(
		const simdpp::float32<VectorSize, E0>& reference_dose, const simdpp::float32<VectorSize, E1>& target_dose,
		const vglobal_gamma_index_params<float, VectorSize>& params)
	{
		auto intermediate = (reference_dose - target_dose);
		return (intermediate * intermediate) / params.dose_difference_squared;
	}

	// -------- DD float64 --------

	template<unsigned VectorSize, typename E0, typename E1>
	inline simdpp::float64<VectorSize> dose_difference(
		const simdpp::float64<VectorSize, E0>& reference_dose, const simdpp::float64<VectorSize, E1>& target_dose,
		const vlocal_gamma_index_params<double, VectorSize>& params)
	{
		auto intermediate = (reference_dose - target_dose) / (reference_dose * params.percentage);
		return intermediate * intermediate;
	}

	template<unsigned VectorSize, typename E0, typename E1>
	inline simdpp::float64<VectorSize> dose_difference(
		const simdpp::float64<VectorSize, E0>& reference_dose, const simdpp::float64<VectorSize, E1>& target_dose,
		const vglobal_gamma_index_params<double, VectorSize>& params)
	{
		auto intermediate = (reference_dose - target_dose);
		return (intermediate * intermediate) / params.dose_difference_squared;
	}

	// -------- DTA float32 --------

	template<unsigned VectorSize, typename E0, typename E1>
	inline simdpp::float32<VectorSize> distance_to_agreement(
		const simdpp::float32<VectorSize, E0>& reference_x,
		const simdpp::float32<VectorSize, E1>& target_x,
		const vlocal_gamma_index_params<float, VectorSize>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		return (intermediate_x * intermediate_x) / params.distance_to_agreement_squared;
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3>
	inline simdpp::float32<VectorSize> distance_to_agreement(
		const simdpp::float32<VectorSize, E0>& reference_x, const simdpp::float32<VectorSize, E1>& reference_y,
		const simdpp::float32<VectorSize, E2>& target_x, const simdpp::float32<VectorSize, E3>& target_y,
		const vlocal_gamma_index_params<float, VectorSize>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y) / params.distance_to_agreement_squared;
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3, typename E4, typename E5>
	inline simdpp::float32<VectorSize> distance_to_agreement(
		const simdpp::float32<VectorSize, E0>& reference_x, const simdpp::float32<VectorSize, E1>& reference_y, const simdpp::float32<VectorSize, E2>& reference_z,
		const simdpp::float32<VectorSize, E3>& target_x, const simdpp::float32<VectorSize, E4>& target_y, const simdpp::float32<VectorSize, E5>& target_z,
		const vlocal_gamma_index_params<float, VectorSize>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		auto intermediate_z = (target_z - reference_z);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y + intermediate_z * intermediate_z) / params.distance_to_agreement_squared;
	}

	template<unsigned VectorSize, typename E0, typename E1>
	inline simdpp::float32<VectorSize> distance_to_agreement(
		const simdpp::float32<VectorSize, E0>& reference_x,
		const simdpp::float32<VectorSize, E1>& target_x,
		const vglobal_gamma_index_params<float, VectorSize>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		return (intermediate_x * intermediate_x) / params.distance_to_agreement_squared;
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3>
	inline simdpp::float32<VectorSize> distance_to_agreement(
		const simdpp::float32<VectorSize, E0>& reference_x, const simdpp::float32<VectorSize, E1>& reference_y,
		const simdpp::float32<VectorSize, E2>& target_x, const simdpp::float32<VectorSize, E3>& target_y,
		const vglobal_gamma_index_params<float, VectorSize>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y) / params.distance_to_agreement_squared;
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3, typename E4, typename E5>
	inline simdpp::float32<VectorSize> distance_to_agreement(
		const simdpp::float32<VectorSize, E0>& reference_x, const simdpp::float32<VectorSize, E1>& reference_y, const simdpp::float32<VectorSize, E2>& reference_z,
		const simdpp::float32<VectorSize, E3>& target_x, const simdpp::float32<VectorSize, E4>& target_y, const simdpp::float32<VectorSize, E5>& target_z,
		const vglobal_gamma_index_params<float, VectorSize>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		auto intermediate_z = (target_z - reference_z);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y + intermediate_z * intermediate_z) / params.distance_to_agreement_squared;
	}

	// -------- DTA float64 --------

	template<unsigned VectorSize, typename E0, typename E1>
	inline simdpp::float64<VectorSize> distance_to_agreement(
		const simdpp::float64<VectorSize, E0>& reference_x,
		const simdpp::float64<VectorSize, E1>& target_x,
		const vlocal_gamma_index_params<double, VectorSize>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		return (intermediate_x * intermediate_x) / params.distance_to_agreement_squared;
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3>
	inline simdpp::float64<VectorSize> distance_to_agreement(
		const simdpp::float64<VectorSize, E0>& reference_x, const simdpp::float64<VectorSize, E1>& reference_y,
		const simdpp::float64<VectorSize, E2>& target_x, const simdpp::float64<VectorSize, E3>& target_y,
		const vlocal_gamma_index_params<double, VectorSize>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y) / params.distance_to_agreement_squared;
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3, typename E4, typename E5>
	inline simdpp::float64<VectorSize> distance_to_agreement(
		const simdpp::float64<VectorSize, E0>& reference_x, const simdpp::float64<VectorSize, E1>& reference_y, const simdpp::float64<VectorSize, E2>& reference_z,
		const simdpp::float64<VectorSize, E3>& target_x, const simdpp::float64<VectorSize, E4>& target_y, const simdpp::float64<VectorSize, E5>& target_z,
		const vlocal_gamma_index_params<double, VectorSize>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		auto intermediate_z = (target_z - reference_z);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y + intermediate_z * intermediate_z) / params.distance_to_agreement_squared;
	}

	template<unsigned VectorSize, typename E0, typename E1>
	inline simdpp::float64<VectorSize> distance_to_agreement(
		const simdpp::float64<VectorSize, E0>& reference_x,
		const simdpp::float64<VectorSize, E1>& target_x,
		const vglobal_gamma_index_params<double, VectorSize>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		return (intermediate_x * intermediate_x) / params.distance_to_agreement_squared;
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3>
	inline simdpp::float64<VectorSize> distance_to_agreement(
		const simdpp::float64<VectorSize, E0>& reference_x, const simdpp::float64<VectorSize, E1>& reference_y,
		const simdpp::float64<VectorSize, E2>& target_x, const simdpp::float64<VectorSize, E3>& target_y,
		const vglobal_gamma_index_params<double, VectorSize>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y) / params.distance_to_agreement_squared;
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3, typename E4, typename E5>
	inline simdpp::float64<VectorSize> distance_to_agreement(
		const simdpp::float64<VectorSize, E0>& reference_x, const simdpp::float64<VectorSize, E1>& reference_y, const simdpp::float64<VectorSize, E2>& reference_z,
		const simdpp::float64<VectorSize, E3>& target_x, const simdpp::float64<VectorSize, E4>& target_y, const simdpp::float64<VectorSize, E5>& target_z,
		const vglobal_gamma_index_params<double, VectorSize>& params)
	{
		auto intermediate_x = (target_x - reference_x);
		auto intermediate_y = (target_y - reference_y);
		auto intermediate_z = (target_z - reference_z);
		return (intermediate_x * intermediate_x + intermediate_y * intermediate_y + intermediate_z * intermediate_z) / params.distance_to_agreement_squared;
	}

	// -------- GI float32 --------

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3>
	inline simdpp::float32<VectorSize> gamma_index(
		const simdpp::float32<VectorSize, E0>& reference_dose,
		const simdpp::float32<VectorSize, E1>& target_dose,
		const simdpp::float32<VectorSize, E2>& reference_x,
		const simdpp::float32<VectorSize, E3>& target_x,
		const vlocal_gamma_index_params<float, VectorSize>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, target_x, params);
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3, typename E4, typename E5>
	inline simdpp::float32<VectorSize> gamma_index(
		const simdpp::float32<VectorSize, E0>& reference_dose,
		const simdpp::float32<VectorSize, E1>& target_dose,
		const simdpp::float32<VectorSize, E2>& reference_x, const simdpp::float32<VectorSize, E3>& reference_y,
		const simdpp::float32<VectorSize, E4>& target_x, const simdpp::float32<VectorSize, E5>& target_y,
		const vlocal_gamma_index_params<float, VectorSize>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, reference_y, target_x, target_y, params);
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3, typename E4, typename E5, typename E6, typename E7>
	inline simdpp::float32<VectorSize> gamma_index(
		const simdpp::float32<VectorSize, E0>& reference_dose,
		const simdpp::float32<VectorSize, E1>& target_dose,
		const simdpp::float32<VectorSize, E2>& reference_x, const simdpp::float32<VectorSize, E3>& reference_y, const simdpp::float32<VectorSize, E4>& reference_z,
		const simdpp::float32<VectorSize, E5>& target_x, const simdpp::float32<VectorSize, E6>& target_y, const simdpp::float32<VectorSize, E7>& target_z,
		const vlocal_gamma_index_params<float, VectorSize>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, reference_y, reference_z, target_x, target_y, target_z, params);
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3>
	inline simdpp::float32<VectorSize> gamma_index(
		const simdpp::float32<VectorSize, E0>& reference_dose,
		const simdpp::float32<VectorSize, E1>& target_dose,
		const simdpp::float32<VectorSize, E2>& reference_x,
		const simdpp::float32<VectorSize, E3>& target_x,
		const vglobal_gamma_index_params<float, VectorSize>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, target_x, params);
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3, typename E4, typename E5>
	inline simdpp::float32<VectorSize> gamma_index(
		const simdpp::float32<VectorSize, E0>& reference_dose,
		const simdpp::float32<VectorSize, E1>& target_dose,
		const simdpp::float32<VectorSize, E2>& reference_x, const simdpp::float32<VectorSize, E3>& reference_y,
		const simdpp::float32<VectorSize, E4>& target_x, const simdpp::float32<VectorSize, E5>& target_y,
		const vglobal_gamma_index_params<float, VectorSize>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, reference_y, target_x, target_y, params);
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3, typename E4, typename E5, typename E6, typename E7>
	inline simdpp::float32<VectorSize> gamma_index(
		const simdpp::float32<VectorSize, E0>& reference_dose,
		const simdpp::float32<VectorSize, E1>& target_dose,
		const simdpp::float32<VectorSize, E2>& reference_x, const simdpp::float32<VectorSize, E3>& reference_y, const simdpp::float32<VectorSize, E4>& reference_z,
		const simdpp::float32<VectorSize, E5>& target_x, const simdpp::float32<VectorSize, E6>& target_y, const simdpp::float32<VectorSize, E7>& target_z,
		const vglobal_gamma_index_params<float, VectorSize>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, reference_y, reference_z, target_x, target_y, target_z, params);
	}

	// -------- GI float64 --------

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3>
	inline simdpp::float64<VectorSize> gamma_index(
		const simdpp::float64<VectorSize, E0>& reference_dose,
		const simdpp::float64<VectorSize, E1>& target_dose,
		const simdpp::float64<VectorSize, E2>& reference_x,
		const simdpp::float64<VectorSize, E3>& target_x,
		const vlocal_gamma_index_params<double, VectorSize>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, target_x, params);
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3, typename E4, typename E5>
	inline simdpp::float64<VectorSize> gamma_index(
		const simdpp::float64<VectorSize, E0>& reference_dose,
		const simdpp::float64<VectorSize, E1>& target_dose,
		const simdpp::float64<VectorSize, E2>& reference_x, const simdpp::float64<VectorSize, E3>& reference_y,
		const simdpp::float64<VectorSize, E4>& target_x, const simdpp::float64<VectorSize, E5>& target_y,
		const vlocal_gamma_index_params<double, VectorSize>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, reference_y, target_x, target_y, params);
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3, typename E4, typename E5, typename E6, typename E7>
	inline simdpp::float64<VectorSize> gamma_index(
		const simdpp::float64<VectorSize, E0>& reference_dose,
		const simdpp::float64<VectorSize, E1>& target_dose,
		const simdpp::float64<VectorSize, E2>& reference_x, const simdpp::float64<VectorSize, E3>& reference_y, const simdpp::float64<VectorSize, E4>& reference_z,
		const simdpp::float64<VectorSize, E5>& target_x, const simdpp::float64<VectorSize, E6>& target_y, const simdpp::float64<VectorSize, E7>& target_z,
		const vlocal_gamma_index_params<double, VectorSize>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, reference_y, reference_z, target_x, target_y, target_z, params);
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3>
	inline simdpp::float64<VectorSize> gamma_index(
		const simdpp::float64<VectorSize, E0>& reference_dose,
		const simdpp::float64<VectorSize, E1>& target_dose,
		const simdpp::float64<VectorSize, E2>& reference_x,
		const simdpp::float64<VectorSize, E3>& target_x,
		const vglobal_gamma_index_params<double, VectorSize>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, target_x, params);
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3, typename E4, typename E5>
	inline simdpp::float64<VectorSize> gamma_index(
		const simdpp::float64<VectorSize, E0>& reference_dose,
		const simdpp::float64<VectorSize, E1>& target_dose,
		const simdpp::float64<VectorSize, E2>& reference_x, const simdpp::float64<VectorSize, E3>& reference_y,
		const simdpp::float64<VectorSize, E4>& target_x, const simdpp::float64<VectorSize, E5>& target_y,
		const vglobal_gamma_index_params<double, VectorSize>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, reference_y, target_x, target_y, params);
	}

	template<unsigned VectorSize, typename E0, typename E1, typename E2, typename E3, typename E4, typename E5, typename E6, typename E7>
	inline simdpp::float64<VectorSize> gamma_index(
		const simdpp::float64<VectorSize, E0>& reference_dose,
		const simdpp::float64<VectorSize, E1>& target_dose,
		const simdpp::float64<VectorSize, E2>& reference_x, const simdpp::float64<VectorSize, E3>& reference_y, const simdpp::float64<VectorSize, E4>& reference_z,
		const simdpp::float64<VectorSize, E5>& target_x, const simdpp::float64<VectorSize, E6>& target_y, const simdpp::float64<VectorSize, E7>& target_z,
		const vglobal_gamma_index_params<double, VectorSize>& params)
	{
		return dose_difference(reference_dose, target_dose, params) + distance_to_agreement(reference_x, reference_y, reference_z, target_x, target_y, target_z, params);
	}
}