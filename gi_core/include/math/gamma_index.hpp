#pragma once

#include <data/image/iwritable_image.hpp>
#include <data/image/dose/idose_image.hpp>
#include <data/image/image_data.hpp>
#include <data/image/dose/image_coordinates.hpp>

// -------- classic --------
#include <math/classic/basic/gamma_index_range.hpp>
#include <math/classic/basic/openmp/gamma_index_range.hpp>
#include <math/classic/vectorized/gamma_index_range.hpp>
#include <math/classic/vectorized/openmp/gamma_index_range.hpp>

#include <execution>

namespace yagit::core::math
{
	using yagit::core::data::data_region;

	using yagit::core::data::iwritable_image;
	using yagit::core::data::iwritable_image_region;

	using yagit::core::data::idose_image;
	using yagit::core::data::idose_image_region;

	using yagit::core::data::image_data;
	using yagit::core::data::image_coordinates;

	template<typename ExecutionPolicy, typename AlgorithmVersion, typename Type, typename Allocator = std::allocator<Type>, typename ParamsType>
	error_code gamma_index(
		AlgorithmVersion algorithm_version,
		ExecutionPolicy policy,
		iwritable_image_region<Type, 1>& gamma_index_output,
		const idose_image_region<Type, 1>& reference, const idose_image_region<Type, 1>& target,
		const ParamsType& params,
		Allocator allocator = Allocator())
	{
		if (gamma_index_output.region() != reference.region())
			return {};//some error code

		error_code error;

		auto gi_output = gamma_index_output.load(gamma_index_output.preferred_data_format(), allocator, error);
		if (!gi_output) return error;

		/*auto r_doses = reference.load(reference.preferred_data_format(), allocator, error);
		if (!r_doses) return error;

		auto r_x_coords = reference.template load_coordinates<0>(reference.template preferred_coordinates_format<0>(), allocator, error);
		if (!r_x_coords) return error;

		auto t_doses = target.load(target.preferred_data_format(), allocator, error);
		if (!t_doses) return error;

		auto t_x_coords = target.template load_coordinates<0>(target.template preferred_coordinates_format<0>(), allocator, error);
		if (!t_x_coords) return error;

		if (error = gamma_index(algorithm_version, policy,
			gi_output->data(), gi_output->data() + gi_output->total_size(),
			r_doses->data(),
			array<const_view<Type>, 1>{ r_x_coords->data() },
			t_doses->data(), t_doses->data() + t_doses->total_size(),
			array<const_view<Type>, 1>{ t_x_coords->data()},
			params))
			return error;*/

		return gamma_index_output.save(*gi_output);
	}

	template<typename ExecutionPolicy, typename AlgorithmVersion, typename Type, typename Allocator = std::allocator<Type>, typename ParamsType>
	error_code gamma_index(
		AlgorithmVersion algorithm_version,
		ExecutionPolicy policy,
		iwritable_image_region<Type, 2>& gamma_index_output,
		const idose_image_region<Type, 2>& reference, const idose_image_region<Type, 2>& target,
		const ParamsType& params,
		Allocator allocator = Allocator())
	{
		if (gamma_index_output.region() != reference.region())
			return {};//some error code

		error_code error;

		auto gi_output = gamma_index_output.load(gamma_index_output.preferred_data_format(), allocator, error);
		if (!gi_output) return error;

		/*auto r_doses = reference.load(reference.preferred_data_format(), allocator, error);
		if (!r_doses) return error;

		auto r_x_coords = reference.template load_coordinates<0>(reference.template preferred_coordinates_format<0>(), allocator, error);
		if (!r_x_coords) return error;

		auto r_y_coords = reference.template load_coordinates<1>(reference.template preferred_coordinates_format<1>(), allocator, error);
		if (!r_y_coords) return error;

		auto t_doses = target.load(target.preferred_data_format(), allocator, error);
		if (!t_doses) return error;

		auto t_x_coords = target.template load_coordinates<0>(target.template preferred_coordinates_format<0>(), allocator, error);
		if (!t_x_coords) return error;

		auto t_y_coords = target.template load_coordinates<1>(target.template preferred_coordinates_format<1>(), allocator, error);
		if (!t_y_coords) return error;

		if (error = gamma_index(algorithm_version, policy,
			gi_output->data(), gi_output->data() + gi_output->total_size(),
			r_doses->data(),
			array<const_view<Type>, 2>{ r_x_coords->data(), r_y_coords->data() },
			t_doses->data(), t_doses->data() + t_doses->total_size(),
			array<const_view<Type>, 2>{ t_x_coords->data(), t_y_coords->data()},
			params))
			return error;*/

		return gamma_index_output.save(*gi_output);
	}

	template<typename ExecutionPolicy, typename AlgorithmVersion, typename Type, typename Allocator = std::allocator<Type>, typename ParamsType>
	error_code gamma_index(
		AlgorithmVersion algorithm_version,
		ExecutionPolicy policy,
		iwritable_image_region<Type, 3>& gamma_index_output,
		const idose_image_region<Type, 3>& reference, const idose_image_region<Type, 3>& target,
		const ParamsType& params,
		Allocator allocator = Allocator())
	{
		if (gamma_index_output.region() != reference.region())
			return {};//some error code

		error_code error;

		auto gi_output = gamma_index_output.load(gamma_index_output.preferred_data_format(), allocator, error);
		if (!gi_output) return error;

		/*auto r_doses = reference.load(reference.preferred_data_format(), allocator, error);
		if (!r_doses) return error;

		auto r_x_coords = reference.template load_coordinates<0>(reference.template preferred_coordinates_format<0>(), allocator, error);
		if (!r_x_coords) return error;

		auto r_y_coords = reference.template load_coordinates<1>(reference.template preferred_coordinates_format<1>(), allocator, error);
		if (!r_y_coords) return error;

		auto r_z_coords = reference.template load_coordinates<2>(reference.template preferred_coordinates_format<2>(), allocator, error);
		if (!r_z_coords) return error;

		auto t_doses = target.load(target.preferred_data_format(), allocator, error);
		if (!t_doses) return error;

		auto t_x_coords = target.template load_coordinates<0>(target.template preferred_coordinates_format<0>(), allocator, error);
		if (!t_x_coords) return error;

		auto t_y_coords = target.template load_coordinates<1>(target.template preferred_coordinates_format<1>(), allocator, error);
		if (!t_y_coords) return error;

		auto t_z_coords = target.template load_coordinates<2>(target.template preferred_coordinates_format<2>(), allocator, error);
		if (!t_z_coords) return error;

		if (error = gamma_index(algorithm_version, policy,
			gi_output->data(), gi_output->data() + gi_output->total_size(),
			r_doses->data(),
			array<const_view<Type>, 3>{ r_x_coords->data(), r_y_coords->data(), r_z_coords->data() },
			t_doses->data(), t_doses->data() + t_doses->total_size(),
			array<const_view<Type>, 3>{ t_x_coords->data(), t_y_coords->data(), t_z_coords->data()},
			params))
			return error;*/

		return gamma_index_output.save(*gi_output);
	}
}