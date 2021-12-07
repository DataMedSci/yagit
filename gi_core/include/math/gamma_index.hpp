#pragma once

#include <data/image/iwritable_image.hpp>
#include <data/image/rtdose/irtdose_image.hpp>
#include <data/image/image_data.hpp>
#include <data/image/rtdose/image_coordinates.hpp>

#include <math/basic/gamma_index_range.hpp>
#include <math/basic/openmp/gamma_index_range.hpp>
#include <math/vectorized/gamma_index_range.hpp>
#include <math/vectorized/openmp/gamma_index_range.hpp>

#include <execution>

namespace yagit::core::math
{
	using yagit::core::data::data_region;

	using yagit::core::data::iwritable_image;
	using yagit::core::data::iwritable_image_region;

	using yagit::core::data::irtdose_image;
	using yagit::core::data::irtdose_image_region;

	using yagit::core::data::image_data;
	using yagit::core::data::image_coordinates;

	namespace detail
	{
		template<size_t Dimensions>
		bool gamma_index_should_split(const data_region<Dimensions>& region)
		{
			return false;
		}

		template<size_t Dimensions, typename Type>
		inline error_code gamma_index_initialize_pass(std::execution::sequenced_policy, view<Type> gamma_index_output, view<Type> gamma_index_output_end)
		{
			return sequenced_gamma_index_implementer<Type, Dimensions>::initialize_pass(gamma_index_output, gamma_index_output_end);
		}

		template<size_t Dimensions, typename Type>
		inline error_code gamma_index_initialize_pass(std::execution::unsequenced_policy, view<Type> gamma_index_output, view<Type> gamma_index_output_end)
		{
			return unsequenced_gamma_index_implementer<Type, Dimensions>::initialize_pass(gamma_index_output, gamma_index_output_end);
		}

		template<size_t Dimensions, typename Type>
		inline error_code gamma_index_initialize_pass(std::execution::parallel_policy, view<Type> gamma_index_output, view<Type> gamma_index_output_end)
		{
			return parallel_gamma_index_implementer<Type, Dimensions>::initialize_pass(gamma_index_output, gamma_index_output_end);
		}

		template<size_t Dimensions, typename Type>
		inline error_code gamma_index_initialize_pass(std::execution::parallel_unsequenced_policy, view<Type> gamma_index_output, view<Type> gamma_index_output_end)
		{
			return parallel_unsequenced_gamma_index_implementer<Type, Dimensions>::initialize_pass(gamma_index_output, gamma_index_output_end);
		}

		template<typename Type, size_t Dimensions>
		inline error_code gamma_index_minimize_pass(
			std::execution::sequenced_policy,
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			array<const_view<Type>, Dimensions> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			array<const_view<Type>, Dimensions> target_coordinates,
			const local_gamma_index_params<Type>& params)
		{
			return sequenced_gamma_index_implementer<Type, Dimensions>::minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}

		template<typename Type, size_t Dimensions>
		inline error_code gamma_index_minimize_pass(
			std::execution::sequenced_policy,
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			array<const_view<Type>, Dimensions> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			array<const_view<Type>, Dimensions> target_coordinates,
			const global_gamma_index_params<Type>& params)
		{
			return sequenced_gamma_index_implementer<Type, Dimensions>::minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}

		template<typename Type, size_t Dimensions>
		inline error_code gamma_index_minimize_pass(
			std::execution::unsequenced_policy,
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			array<const_view<Type>, Dimensions> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			array<const_view<Type>, Dimensions> target_coordinates,
			const local_gamma_index_params<Type>& params)
		{
			return unsequenced_gamma_index_implementer<Type, Dimensions>::minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}

		template<typename Type, size_t Dimensions>
		inline error_code gamma_index_minimize_pass(
			std::execution::unsequenced_policy,
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			array<const_view<Type>, Dimensions> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			array<const_view<Type>, Dimensions> target_coordinates,
			const global_gamma_index_params<Type>& params)
		{
			return unsequenced_gamma_index_implementer<Type, Dimensions>::minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}

		template<typename Type, size_t Dimensions>
		inline error_code gamma_index_minimize_pass(
			std::execution::parallel_policy,
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			array<const_view<Type>, Dimensions> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			array<const_view<Type>, Dimensions> target_coordinates,
			const local_gamma_index_params<Type>& params)
		{
			return parallel_gamma_index_implementer<Type, Dimensions>::minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}

		template<typename Type, size_t Dimensions>
		inline error_code gamma_index_minimize_pass(
			std::execution::parallel_policy,
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			array<const_view<Type>, Dimensions> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			array<const_view<Type>, Dimensions> target_coordinates,
			const global_gamma_index_params<Type>& params)
		{
			return parallel_gamma_index_implementer<Type, Dimensions>::minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}

		template<typename Type, size_t Dimensions>
		inline error_code gamma_index_minimize_pass(
			std::execution::parallel_unsequenced_policy,
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			array<const_view<Type>, Dimensions> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			array<const_view<Type>, Dimensions> target_coordinates,
			const local_gamma_index_params<Type>& params)
		{
			return parallel_unsequenced_gamma_index_implementer<Type, Dimensions>::minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}

		template<typename Type, size_t Dimensions>
		inline error_code gamma_index_minimize_pass(
			std::execution::parallel_unsequenced_policy,
			view<Type> output, view<Type> output_end,
			const_view<Type> reference_doses,
			array<const_view<Type>, Dimensions> reference_coordinates,
			const_view<Type> target_doses, const_view<Type> target_doses_end,
			array<const_view<Type>, Dimensions> target_coordinates,
			const global_gamma_index_params<Type>& params)
		{
			return parallel_unsequenced_gamma_index_implementer<Type, Dimensions>::minimize_pass(
				output, output_end,
				reference_doses,
				reference_coordinates,
				target_doses, target_doses_end,
				target_coordinates,
				params);
		}

		template<size_t Dimensions, typename Type>
		inline error_code gamma_index_finalize_pass(std::execution::sequenced_policy, view<Type> gamma_index_output, view<Type> gamma_index_output_end)
		{
			return sequenced_gamma_index_implementer<Type, Dimensions>::finalize_pass(gamma_index_output, gamma_index_output_end);
		}

		template<size_t Dimensions, typename Type>
		inline error_code gamma_index_finalize_pass(std::execution::unsequenced_policy, view<Type> gamma_index_output, view<Type> gamma_index_output_end)
		{
			return unsequenced_gamma_index_implementer<Type, Dimensions>::finalize_pass(gamma_index_output, gamma_index_output_end);
		}

		template<size_t Dimensions, typename Type>
		inline error_code gamma_index_finalize_pass(std::execution::parallel_policy, view<Type> gamma_index_output, view<Type> gamma_index_output_end)
		{
			return parallel_gamma_index_implementer<Type, Dimensions>::finalize_pass(gamma_index_output, gamma_index_output_end);
		}

		template<size_t Dimensions, typename Type>
		inline error_code gamma_index_finalize_pass(std::execution::parallel_unsequenced_policy, view<Type> gamma_index_output, view<Type> gamma_index_output_end)
		{
			return parallel_unsequenced_gamma_index_implementer<Type, Dimensions>::finalize_pass(gamma_index_output, gamma_index_output_end);
		}

		// -------- image_data, image_coordinates --------

		template<typename Type, typename ExecutionPolicy>
		error_code gamma_index(
			ExecutionPolicy policy,
			image_data<Type, 1>& gamma_index_output,
			const image_data<Type, 1>& reference_doses,
			const image_coordinates<Type, 1>& reference_x_coordinates,
			const image_data<Type, 1>& target_doses,
			const image_coordinates<Type, 1>& target_x_coordinate,
			const local_gamma_index_params<Type>& params
		)
		{
			return gamma_index_minimize_pass(
				policy,
				gamma_index_output.data(), gamma_index_output.data() + gamma_index_output.total_size(),
				reference_doses.data(),
				array<const_view<Type>, 1>{ reference_x_coordinates.data() },
				target_doses.data(), target_doses.data() + target_doses.total_size(),
				array<const_view<Type>, 1>{ target_x_coordinate.data() },
				params);
		}

		template<typename Type, typename ExecutionPolicy>
		error_code gamma_index(
			ExecutionPolicy policy,
			image_data<Type, 2>& gamma_index_output,
			const image_data<Type, 2>& reference_doses,
			const image_coordinates<Type, 2>& reference_x_coordinates, const image_coordinates<Type, 2>& reference_y_coordinates,
			const image_data<Type, 2>& target_doses,
			const image_coordinates<Type, 2>& target_x_coordinate, const image_coordinates<Type, 2>& target_y_coordinate,
			const local_gamma_index_params<Type>& params
		)
		{
			return gamma_index_minimize_pass(
				policy,
				gamma_index_output.data(), gamma_index_output.data() + gamma_index_output.total_size(),
				reference_doses.data(),
				array<const_view<Type>, 2>{ reference_x_coordinates.data(), reference_y_coordinates.data() },
				target_doses.data(), target_doses.data() + target_doses.total_size(),
				array<const_view<Type>, 2>{ target_x_coordinate.data(), target_y_coordinate.data() },
				params);
		}

		template<typename Type, typename ExecutionPolicy>
		error_code gamma_index(
			ExecutionPolicy policy,
			image_data<Type, 3>& gamma_index_output,
			const image_data<Type, 3>& reference_doses,
			const image_coordinates<Type, 3>& reference_x_coordinates, const image_coordinates<Type, 3>& reference_y_coordinates, const image_coordinates<Type, 3>& reference_z_coordinates,
			const image_data<Type, 3>& target_doses,
			const image_coordinates<Type, 3>& target_x_coordinate, const image_coordinates<Type, 3>& target_y_coordinate, const image_coordinates<Type, 3>& target_z_coordinate,
			const local_gamma_index_params<Type>& params
		)
		{
			return gamma_index_minimize_pass(
				policy,
				gamma_index_output.data(), gamma_index_output.data() + gamma_index_output.total_size(),
				reference_doses.data(),
				array<const_view<Type>, 3>{ reference_x_coordinates.data(), reference_y_coordinates.data(), reference_z_coordinates.data() },
				target_doses.data(), target_doses.data() + target_doses.total_size(),
				array<const_view<Type>, 3>{ target_x_coordinate.data(), target_y_coordinate.data(), target_z_coordinate.data() },
				params);
		}

		template<typename Type, typename ExecutionPolicy>
		error_code gamma_index(
			ExecutionPolicy policy,
			image_data<Type, 1>& gamma_index_output,
			const image_data<Type, 1>& reference_doses,
			const image_coordinates<Type, 1>& reference_x_coordinates,
			const image_data<Type, 1>& target_doses,
			const image_coordinates<Type, 1>& target_x_coordinate,
			const global_gamma_index_params<Type>& params
		)
		{
			return gamma_index_minimize_pass(
				policy,
				gamma_index_output.data(), gamma_index_output.data() + gamma_index_output.total_size(),
				reference_doses.data(),
				array<const_view<Type>, 1>{ reference_x_coordinates.data() },
				target_doses.data(), target_doses.data() + target_doses.total_size(),
				array<const_view<Type>, 1>{ target_x_coordinate.data() },
				params);
		}

		template<typename Type, typename ExecutionPolicy>
		error_code gamma_index(
			ExecutionPolicy policy,
			image_data<Type, 2>& gamma_index_output,
			const image_data<Type, 2>& reference_doses,
			const image_coordinates<Type, 2>& reference_x_coordinates, const image_coordinates<Type, 2>& reference_y_coordinates,
			const image_data<Type, 2>& target_doses,
			const image_coordinates<Type, 2>& target_x_coordinate, const image_coordinates<Type, 2>& target_y_coordinate,
			const global_gamma_index_params<Type>& params
		)
		{
			return gamma_index_minimize_pass(
				policy,
				gamma_index_output.data(), gamma_index_output.data() + gamma_index_output.total_size(),
				reference_doses.data(),
				array<const_view<Type>, 2>{ reference_x_coordinates.data(), reference_y_coordinates.data() },
				target_doses.data(), target_doses.data() + target_doses.total_size(),
				array<const_view<Type>, 2>{ target_x_coordinate.data(), target_y_coordinate.data() },
				params);
		}

		template<typename Type, typename ExecutionPolicy>
		error_code gamma_index(
			ExecutionPolicy policy,
			image_data<Type, 3>& gamma_index_output,
			const image_data<Type, 3>& reference_doses,
			const image_coordinates<Type, 3>& reference_x_coordinates, const image_coordinates<Type, 3>& reference_y_coordinates, const image_coordinates<Type, 3>& reference_z_coordinates,
			const image_data<Type, 3>& target_doses,
			const image_coordinates<Type, 3>& target_x_coordinate, const image_coordinates<Type, 3>& target_y_coordinate, const image_coordinates<Type, 3>& target_z_coordinate,
			const global_gamma_index_params<Type>& params
		)
		{
			return gamma_index_minimize_pass(
				policy,
				gamma_index_output.data(), gamma_index_output.data() + gamma_index_output.total_size(),
				reference_doses.data(),
				array<const_view<Type>, 3>{ reference_x_coordinates.data(), reference_y_coordinates.data(), reference_z_coordinates.data() },
				target_doses.data(), target_doses.data() + target_doses.total_size(),
				array<const_view<Type>, 3>{ target_x_coordinate.data(), target_y_coordinate.data(), target_z_coordinate.data() },
				params);
		}

		// -------- reference image_data, image_coordinates, target as iimage --------

		constexpr size_t splitting_factor = 2;

		constexpr std::array<data_region<1>, splitting_factor> split_region(const data_region<1>& current_region)
		{
			data_region<1> r0{ {0},{current_region.size.sizes[0] / splitting_factor} };
			data_region<1> r1{ {r0.size.sizes[0]},{current_region.size.sizes[0] - r0.size.sizes[0]} };

			return { r0,r1 };
		}

		constexpr std::array<data_region<2>, splitting_factor * splitting_factor> split_region(const data_region<2>& current_region)
		{
			data_region<2> r00{ {0,0},{current_region.size.sizes[0] / splitting_factor, current_region.size.sizes[1] / splitting_factor} };
			data_region<2> r01{ {r00.size.sizes[0], 0},{current_region.size.sizes[0] - r00.size.sizes[0], r00.size.sizes[1]} };
			data_region<2> r10{ {0, r00.size.sizes[1]},{r00.size.sizes[0], current_region.size.sizes[1] - r00.size.sizes[1]} };
			data_region<2> r11{ {r01.offset.offset[0], r10.offset.offset[1]},{r01.size.sizes[0], r10.size.sizes[1]} };

			return { r00,r01,r10,r11 };
		}

		constexpr std::array<data_region<3>, splitting_factor * splitting_factor * splitting_factor> split_region(const data_region<3>& current_region)
		{
			data_region<3> r000{ {0,0,0},{current_region.size.sizes[0] / splitting_factor, current_region.size.sizes[1] / splitting_factor, current_region.size.sizes[2] / splitting_factor} };
			data_region<3> r001{ {r000.size.sizes[0], 0, 0},{current_region.size.sizes[0] - r000.size.sizes[0], r000.size.sizes[1], r000.size.sizes[2]} };
			data_region<3> r010{ {0, r000.size.sizes[1], 0},{r000.size.sizes[0], current_region.size.sizes[1] - r000.size.sizes[1], r000.size.sizes[2]} };
			data_region<3> r011{ {r001.offset.offset[0], r010.offset.offset[1], 0},{r001.size.sizes[0], r010.size.sizes[1], r000.size.sizes[2]} };

			data_region<3> r100{ {0,0,r000.size.sizes[2]},{r000.size.sizes[0], r000.size.sizes[0], current_region.size.sizes[2] - r000.size.sizes[2]} };
			data_region<3> r101{ {r000.size.sizes[0], 0, r000.size.sizes[2]},{current_region.size.sizes[0] - r000.size.sizes[0], r000.size.sizes[1], r100.size.sizes[2]} };
			data_region<3> r110{ {0, r000.size.sizes[1], r000.size.sizes[2]},{r000.size.sizes[0], current_region.size.sizes[1] - r000.size.sizes[1], r100.size.sizes[2]} };
			data_region<3> r111{ {r001.offset.offset[0], r010.offset.offset[1], r000.size.sizes[2]},{r001.size.sizes[0], r010.size.sizes[1], r100.size.sizes[2]} };

			return { r000,r001,r010,r011,r100,r101,r110,r111 };
		}

		template<typename ExecutionPolicy, typename Type, typename Allocator = std::allocator<Type>>
		std::error_code gamma_index(
			ExecutionPolicy policy,
			image_data<Type, 1>& gamma_index_output,
			const image_data<Type, 1>& reference_doses,
			const image_coordinates<Type, 1>& reference_x_coordinates,
			const irtdose_image_region<Type, 1>& target,
			const local_gamma_index_params<Type>& params,
			Allocator allocator = Allocator())
		{
			if (detail::gamma_index_should_split(target.region()))
			{
				for (const auto& region : split_region(target.region()))
				{
					std::error_code error;

					auto target_subregion = target.subregion(region, error);
					if (!target_subregion) return error;

					error = gamma_index(
						policy,
						gamma_index_output,
						reference_doses,
						reference_x_coordinates,
						*target_subregion,
						params,
						allocator
					);
					if (error) return error;
				}
				return {};
			}
			else
			{
				std::error_code error = {};

				auto t_doses = target.load(target.preferred_data_format(), allocator, error);
				if (!t_doses) return error;

				auto t_x_coords = target.template load_coordinates<0>(target.template preferred_coordinates_format<0>(), allocator, error);
				if (!t_x_coords) return error;


				gamma_index(
					policy,
					gamma_index_output,
					reference_doses,
					reference_x_coordinates,
					*t_doses,
					*t_x_coords,
					params);

				return {};
			}
		}

		template<typename ExecutionPolicy, typename Type, typename Allocator = std::allocator<Type>>
		std::error_code gamma_index(
			ExecutionPolicy policy,
			image_data<Type, 2>& gamma_index_output,
			const image_data<Type, 2>& reference_doses,
			const image_coordinates<Type, 2>& reference_x_coordinates, const image_coordinates<Type, 2>& reference_y_coordinates,
			const irtdose_image_region<Type, 2>& target,
			const local_gamma_index_params<Type>& params,
			Allocator allocator = Allocator())
		{
			if (detail::gamma_index_should_split(target.region()))
			{
				for (const auto& region : split_region(target.region()))
				{
					std::error_code error;

					auto target_subregion = target.subregion(region, error);
					if (!target_subregion) return error;

					error = gamma_index(
						policy,
						gamma_index_output,
						reference_doses,
						reference_x_coordinates, reference_y_coordinates,
						*target_subregion,
						params,
						allocator
					);
					if (error) return error;
				}
				return {};
			}
			else
			{
				std::error_code error = {};

				auto t_doses = target.load(target.preferred_data_format(), allocator, error);
				if (!t_doses) return error;

				auto t_x_coords = target.template load_coordinates<0>(target.template preferred_coordinates_format<0>(), allocator, error);
				if (!t_x_coords) return error;

				auto t_y_coords = target.template load_coordinates<1>(target.template preferred_coordinates_format<1>(), allocator, error);
				if (!t_y_coords) return error;

				gamma_index(
					policy,
					gamma_index_output,
					reference_doses,
					reference_x_coordinates, reference_y_coordinates,
					*t_doses,
					*t_x_coords, *t_y_coords,
					params);

				return {};
			}
		}

		template<typename ExecutionPolicy, typename Type, typename Allocator = std::allocator<Type>>
		std::error_code gamma_index(
			ExecutionPolicy policy,
			image_data<Type, 3>& gamma_index_output,
			const image_data<Type, 3>& reference_doses,
			const image_coordinates<Type, 3>& reference_x_coordinates, const image_coordinates<Type, 3>& reference_y_coordinates, const image_coordinates<Type, 3>& reference_z_coordinates,
			const irtdose_image_region<Type, 3>& target,
			const local_gamma_index_params<Type>& params,
			Allocator allocator = Allocator())
		{
			if (detail::gamma_index_should_split(target.region()))
			{
				for (const auto& region : split_region(target.region()))
				{
					std::error_code error;

					auto target_subregion = target.subregion(region, error);
					if (!target_subregion) return error;

					error = gamma_index(
						policy,
						gamma_index_output,
						reference_doses,
						reference_x_coordinates, reference_y_coordinates, reference_z_coordinates,
						*target_subregion,
						params,
						allocator
					);
					if (error) return error;
				}
				return {};
			}
			else
			{
				std::error_code error = {};

				auto t_doses = target.load(target.preferred_data_format(), allocator, error);
				if (!t_doses) return error;

				auto t_x_coords = target.template load_coordinates<0>(target.template preferred_coordinates_format<0>(), allocator, error);
				if (!t_x_coords) return error;

				auto t_y_coords = target.template load_coordinates<1>(target.template preferred_coordinates_format<1>(), allocator, error);
				if (!t_y_coords) return error;

				auto t_z_coords = target.template load_coordinates<2>(target.template preferred_coordinates_format<2>(), allocator, error);
				if (!t_z_coords) return error;

				gamma_index(
					policy,
					gamma_index_output,
					reference_doses,
					reference_x_coordinates, reference_y_coordinates, reference_z_coordinates,
					*t_doses,
					*t_x_coords, *t_y_coords, *t_z_coords,
					params);

				return {};
			}
		}

		template<typename ExecutionPolicy, typename Type, typename Allocator = std::allocator<Type>>
		std::error_code gamma_index(
			ExecutionPolicy policy,
			image_data<Type, 1>& gamma_index_output,
			const image_data<Type, 1>& reference_doses,
			const image_coordinates<Type, 1>& reference_x_coordinates,
			const irtdose_image_region<Type, 1>& target,
			const global_gamma_index_params<Type>& params,
			Allocator allocator = Allocator())
		{
			if (detail::gamma_index_should_split(target.region()))
			{
				for (const auto& region : split_region(target.region()))
				{
					std::error_code error;

					auto target_subregion = target.subregion(region, error);
					if (!target_subregion) return error;

					error = gamma_index(
						policy,
						gamma_index_output,
						reference_doses,
						reference_x_coordinates,
						*target_subregion,
						params,
						allocator
					);
					if (error) return error;
				}
				return {};
			}
			else
			{
				std::error_code error = {};

				auto t_doses = target.load(target.preferred_data_format(), allocator, error);
				if (!t_doses) return error;

				auto t_x_coords = target.template load_coordinates<0>(target.template preferred_coordinates_format<0>(), allocator, error);
				if (!t_x_coords) return error;


				gamma_index(
					policy,
					gamma_index_output,
					reference_doses,
					reference_x_coordinates,
					*t_doses,
					*t_x_coords,
					params);

				return {};
			}
		}

		template<typename ExecutionPolicy, typename Type, typename Allocator = std::allocator<Type>>
		std::error_code gamma_index(
			ExecutionPolicy policy,
			image_data<Type, 2>& gamma_index_output,
			const image_data<Type, 2>& reference_doses,
			const image_coordinates<Type, 2>& reference_x_coordinates, const image_coordinates<Type, 2>& reference_y_coordinates,
			const irtdose_image_region<Type, 2>& target,
			const global_gamma_index_params<Type>& params,
			Allocator allocator = Allocator())
		{
			if (detail::gamma_index_should_split(target.region()))
			{
				for (const auto& region : split_region(target.region()))
				{
					std::error_code error;

					auto target_subregion = target.subregion(region, error);
					if (!target_subregion) return error;

					error = gamma_index(
						policy,
						gamma_index_output,
						reference_doses,
						reference_x_coordinates, reference_y_coordinates,
						*target_subregion,
						params,
						allocator
					);
					if (error) return error;
				}
				return {};
			}
			else
			{
				std::error_code error = {};

				auto t_doses = target.load(target.preferred_data_format(), allocator, error);
				if (!t_doses) return error;

				auto t_x_coords = target.template load_coordinates<0>(target.template preferred_coordinates_format<0>(), allocator, error);
				if (!t_x_coords) return error;

				auto t_y_coords = target.template load_coordinates<1>(target.template preferred_coordinates_format<1>(), allocator, error);
				if (!t_y_coords) return error;

				gamma_index(
					policy,
					gamma_index_output,
					reference_doses,
					reference_x_coordinates, reference_y_coordinates,
					*t_doses,
					*t_x_coords, *t_y_coords,
					params);

				return {};
			}
		}

		template<typename ExecutionPolicy, typename Type, typename Allocator = std::allocator<Type>>
		std::error_code gamma_index(
			ExecutionPolicy policy,
			image_data<Type, 3>& gamma_index_output,
			const image_data<Type, 3>& reference_doses,
			const image_coordinates<Type, 3>& reference_x_coordinates, const image_coordinates<Type, 3>& reference_y_coordinates, const image_coordinates<Type, 3>& reference_z_coordinates,
			const irtdose_image_region<Type, 3>& target,
			const global_gamma_index_params<Type>& params,
			Allocator allocator = Allocator())
		{
			if (detail::gamma_index_should_split(target.region()))
			{
				for (const auto& region : split_region(target.region()))
				{
					std::error_code error;

					auto target_subregion = target.subregion(region, error);
					if (!target_subregion) return error;

					error = gamma_index(
						policy,
						gamma_index_output,
						reference_doses,
						reference_x_coordinates, reference_y_coordinates, reference_z_coordinates,
						*target_subregion,
						params,
						allocator
					);
					if (error) return error;
				}
				return {};
			}
			else
			{
				std::error_code error = {};

				auto t_doses = target.load(target.preferred_data_format(), allocator, error);
				if (!t_doses) return error;

				auto t_x_coords = target.template load_coordinates<0>(target.template preferred_coordinates_format<0>(), allocator, error);
				if (!t_x_coords) return error;

				auto t_y_coords = target.template load_coordinates<1>(target.template preferred_coordinates_format<1>(), allocator, error);
				if (!t_y_coords) return error;

				auto t_z_coords = target.template load_coordinates<2>(target.template preferred_coordinates_format<2>(), allocator, error);
				if (!t_z_coords) return error;

				gamma_index(
					policy,
					gamma_index_output,
					reference_doses,
					reference_x_coordinates, reference_y_coordinates, reference_z_coordinates,
					*t_doses,
					*t_x_coords, *t_y_coords, *t_z_coords,
					params);

				return {};
			}
		}
	}

	template<typename ExecutionPolicy, typename Type, typename Allocator = std::allocator<Type>>
	error_code gamma_index(
		ExecutionPolicy policy,
		iwritable_image_region<Type, 1>& gamma_index_output,
		const irtdose_image_region<Type, 1>& reference, const irtdose_image_region<Type, 1>& target,
		const local_gamma_index_params<Type>& params,
		Allocator allocator = Allocator())
	{
		if (gamma_index_output.region() != reference.region())
			return {};//some error code

		if (detail::gamma_index_should_split(reference.region()))
		{
			for (const auto& region : detail::split_region(reference.region()))
			{
				std::error_code error;

				auto gamma_index_output_subregion = gamma_index_output.subregion(region, error);
				if (!gamma_index_output_subregion) return error;

				auto reference_subregion = target.subregion(region, error);
				if (!reference_subregion) return error;

				error = gamma_index(
					policy,
					*gamma_index_output_subregion,
					*reference_subregion,
					target,
					params,
					allocator
				);
				if (error) return error;
			}
			return {};
		}
		else
		{
			std::error_code error = {};

			auto gi_output = gamma_index_output.load(gamma_index_output.preferred_data_format(), allocator, error);
			if (!gi_output) return error;

			error = detail::gamma_index_initialize_pass<1>(policy, gi_output->data(), gi_output->data() + gi_output->real_total_size());
			if (error) return error;

			auto r_doses = reference.load(reference.preferred_data_format(), allocator, error);
			if (!r_doses) return error;

			auto r_x_coords = reference.template load_coordinates<0>(reference.template preferred_coordinates_format<0>(), allocator, error);
			if (!r_x_coords) return error;

			error = detail::gamma_index(
				policy,
				*gi_output,
				*r_doses,
				*r_x_coords,
				target,
				params,
				allocator);
			if (error) return error;

			error = detail::gamma_index_finalize_pass<1>(policy, gi_output->data(), gi_output->data() + gi_output->real_total_size());
			if (error) return error;

			return gamma_index_output.save(*gi_output);
		}
	}

	template<typename ExecutionPolicy, typename Type, typename Allocator = std::allocator<Type>>
	error_code gamma_index(
		ExecutionPolicy policy,
		iwritable_image_region<Type, 2>& gamma_index_output,
		const irtdose_image_region<Type, 2>& reference, const irtdose_image_region<Type, 2>& target,
		const local_gamma_index_params<Type>& params,
		Allocator allocator = Allocator())
	{
		if (gamma_index_output.region() != reference.region())
			return {};//some error code

		if (detail::gamma_index_should_split(reference.region()))
		{
			for (const auto& region : detail::split_region(reference.region()))
			{
				std::error_code error;

				auto gamma_index_output_subregion = gamma_index_output.subregion(region, error);
				if (!gamma_index_output_subregion) return error;

				auto reference_subregion = target.subregion(region, error);
				if (!reference_subregion) return error;

				error = gamma_index(
					policy,
					*gamma_index_output_subregion,
					*reference_subregion,
					target,
					params,
					allocator
				);
				if (error) return error;
			}
			return {};
		}
		else
		{
			std::error_code error = {};

			auto gi_output = gamma_index_output.load(gamma_index_output.preferred_data_format(), allocator, error);
			if (!gi_output) return error;

			error = detail::gamma_index_initialize_pass<2>(policy, gi_output->data(), gi_output->data() + gi_output->real_total_size());
			if (error) return error;

			auto r_doses = reference.load(reference.preferred_data_format(), allocator, error);
			if (!r_doses) return error;

			auto r_x_coords = reference.template load_coordinates<0>(reference.template preferred_coordinates_format<0>(), allocator, error);
			if (!r_x_coords) return error;

			auto r_y_coords = reference.template load_coordinates<1>(reference.template preferred_coordinates_format<1>(), allocator, error);
			if (!r_y_coords) return error;

			error = detail::gamma_index(
				policy,
				*gi_output,
				*r_doses,
				*r_x_coords, *r_y_coords,
				target,
				params,
				allocator);
			if (error) return error;

			error = detail::gamma_index_finalize_pass<2>(policy, gi_output->data(), gi_output->data() + gi_output->real_total_size());
			if (error) return error;

			return gamma_index_output.save(*gi_output);
		}
	}

	template<typename ExecutionPolicy, typename Type, typename Allocator = std::allocator<Type>>
	error_code gamma_index(
		ExecutionPolicy policy,
		iwritable_image_region<Type, 3>& gamma_index_output,
		const irtdose_image_region<Type, 3>& reference, const irtdose_image_region<Type, 3>& target,
		const local_gamma_index_params<Type>& params,
		Allocator allocator = Allocator())
	{
		if (gamma_index_output.region() != reference.region())
			return {};//some error code

		if (detail::gamma_index_should_split(reference.region()))
		{
			for (const auto& region : detail::split_region(reference.region()))
			{
				std::error_code error;

				auto gamma_index_output_subregion = gamma_index_output.subregion(region, error);
				if (!gamma_index_output_subregion) return error;

				auto reference_subregion = target.subregion(region, error);
				if (!reference_subregion) return error;

				error = gamma_index(
					policy,
					*gamma_index_output_subregion,
					*reference_subregion,
					target,
					params,
					allocator
				);
				if (error) return error;
			}
			return {};
		}
		else
		{
			std::error_code error = {};

			auto gi_output = gamma_index_output.load(gamma_index_output.preferred_data_format(), allocator, error);
			if (!gi_output) return error;

			error = detail::gamma_index_initialize_pass<3>(policy, gi_output->data(), gi_output->data() + gi_output->real_total_size());
			if (error) return error;

			auto r_doses = reference.load(reference.preferred_data_format(), allocator, error);
			if (!r_doses) return error;

			auto r_x_coords = reference.template load_coordinates<0>(reference.template preferred_coordinates_format<0>(), allocator, error);
			if (!r_x_coords) return error;

			auto r_y_coords = reference.template load_coordinates<1>(reference.template preferred_coordinates_format<1>(), allocator, error);
			if (!r_y_coords) return error;

			auto r_z_coords = reference.template load_coordinates<2>(reference.template preferred_coordinates_format<2>(), allocator, error);
			if (!r_z_coords) return error;

			error = detail::gamma_index(
				policy,
				*gi_output,
				*r_doses,
				*r_x_coords, *r_y_coords, *r_z_coords,
				target,
				params,
				allocator);
			if (error) return error;

			error = detail::gamma_index_finalize_pass<3>(policy, gi_output->data(), gi_output->data() + gi_output->real_total_size());
			if (error) return error;

			return gamma_index_output.save(*gi_output);
		}
	}

	template<typename ExecutionPolicy, typename Type, typename Allocator = std::allocator<Type>>
	error_code gamma_index(
		ExecutionPolicy policy,
		iwritable_image_region<Type, 1>& gamma_index_output,
		const irtdose_image_region<Type, 1>& reference, const irtdose_image_region<Type, 1>& target,
		const global_gamma_index_params<Type>& params,
		Allocator allocator = Allocator())
	{
		if (gamma_index_output.region() != reference.region())
			return {};//some error code

		if (detail::gamma_index_should_split(reference.region()))
		{
			for (const auto& region : detail::split_region(reference.region()))
			{
				std::error_code error;

				auto gamma_index_output_subregion = gamma_index_output.subregion(region, error);
				if (!gamma_index_output_subregion) return error;

				auto reference_subregion = target.subregion(region, error);
				if (!reference_subregion) return error;

				error = gamma_index(
					policy,
					*gamma_index_output_subregion,
					*reference_subregion,
					target,
					params,
					allocator
				);
				if (error) return error;
			}
			return {};
		}
		else
		{
			std::error_code error = {};

			auto gi_output = gamma_index_output.load(gamma_index_output.preferred_data_format(), allocator, error);
			if (!gi_output) return error;

			error = detail::gamma_index_initialize_pass<1>(policy, gi_output->data(), gi_output->data() + gi_output->real_total_size());
			if (error) return error;

			auto r_doses = reference.load(reference.preferred_data_format(), allocator, error);
			if (!r_doses) return error;

			auto r_x_coords = reference.template load_coordinates<0>(reference.template preferred_coordinates_format<0>(), allocator, error);
			if (!r_x_coords) return error;

			error = detail::gamma_index(
				policy,
				*gi_output,
				*r_doses,
				*r_x_coords,
				target,
				params,
				allocator);
			if (error) return error;

			error = detail::gamma_index_finalize_pass<1>(policy, gi_output->data(), gi_output->data() + gi_output->real_total_size());
			if (error) return error;

			return gamma_index_output.save(*gi_output);
		}
	}

	template<typename ExecutionPolicy, typename Type, typename Allocator = std::allocator<Type>>
	error_code gamma_index(
		ExecutionPolicy policy,
		iwritable_image_region<Type, 2>& gamma_index_output,
		const irtdose_image_region<Type, 2>& reference, const irtdose_image_region<Type, 2>& target,
		const global_gamma_index_params<Type>& params,
		Allocator allocator = Allocator())
	{
		if (gamma_index_output.region() != reference.region())
			return {};//some error code

		if (detail::gamma_index_should_split(reference.region()))
		{
			for (const auto& region : detail::split_region(reference.region()))
			{
				std::error_code error;

				auto gamma_index_output_subregion = gamma_index_output.subregion(region, error);
				if (!gamma_index_output_subregion) return error;

				auto reference_subregion = target.subregion(region, error);
				if (!reference_subregion) return error;

				error = gamma_index(
					policy,
					*gamma_index_output_subregion,
					*reference_subregion,
					target,
					params,
					allocator
				);
				if (error) return error;
			}
			return {};
		}
		else
		{
			std::error_code error = {};

			auto gi_output = gamma_index_output.load(gamma_index_output.preferred_data_format(), allocator, error);
			if (!gi_output) return error;

			error = detail::gamma_index_initialize_pass<2>(policy, gi_output->data(), gi_output->data() + gi_output->real_total_size());
			if (error) return error;

			auto r_doses = reference.load(reference.preferred_data_format(), allocator, error);
			if (!r_doses) return error;

			auto r_x_coords = reference.template load_coordinates<0>(reference.template preferred_coordinates_format<0>(), allocator, error);
			if (!r_x_coords) return error;

			auto r_y_coords = reference.template load_coordinates<1>(reference.template preferred_coordinates_format<1>(), allocator, error);
			if (!r_y_coords) return error;

			error = detail::gamma_index(
				policy,
				*gi_output,
				*r_doses,
				*r_x_coords, *r_y_coords,
				target,
				params,
				allocator);
			if (error) return error;

			error = detail::gamma_index_finalize_pass<2>(policy, gi_output->data(), gi_output->data() + gi_output->real_total_size());
			if (error) return error;

			return gamma_index_output.save(*gi_output);
		}
	}

	template<typename ExecutionPolicy, typename Type, typename Allocator = std::allocator<Type>>
	error_code gamma_index(
		ExecutionPolicy policy,
		iwritable_image_region<Type, 3>& gamma_index_output,
		const irtdose_image_region<Type, 3>& reference, const irtdose_image_region<Type, 3>& target,
		const global_gamma_index_params<Type>& params,
		Allocator allocator = Allocator())
	{
		if (gamma_index_output.region() != reference.region())
			return {};//some error code

		if (detail::gamma_index_should_split(reference.region()))
		{
			for (const auto& region : detail::split_region(reference.region()))
			{
				std::error_code error;

				auto gamma_index_output_subregion = gamma_index_output.subregion(region, error);
				if (!gamma_index_output_subregion) return error;

				auto reference_subregion = target.subregion(region, error);
				if (!reference_subregion) return error;

				error = gamma_index(
					policy,
					*gamma_index_output_subregion,
					*reference_subregion,
					target,
					params,
					allocator
				);
				if (error) return error;
			}
			return {};
		}
		else
		{
			std::error_code error = {};

			auto gi_output = gamma_index_output.load(gamma_index_output.preferred_data_format(), allocator, error);
			if (!gi_output) return error;

			error = detail::gamma_index_initialize_pass<3>(policy, gi_output->data(), gi_output->data() + gi_output->real_total_size());
			if (error) return error;

			auto r_doses = reference.load(reference.preferred_data_format(), allocator, error);
			if (!r_doses) return error;

			auto r_x_coords = reference.template load_coordinates<0>(reference.template preferred_coordinates_format<0>(), allocator, error);
			if (!r_x_coords) return error;

			auto r_y_coords = reference.template load_coordinates<1>(reference.template preferred_coordinates_format<1>(), allocator, error);
			if (!r_y_coords) return error;

			auto r_z_coords = reference.template load_coordinates<2>(reference.template preferred_coordinates_format<2>(), allocator, error);
			if (!r_z_coords) return error;

			error = detail::gamma_index(
				policy,
				*gi_output,
				*r_doses,
				*r_x_coords, *r_y_coords, *r_z_coords,
				target,
				params,
				allocator);
			if (error) return error;

			error = detail::gamma_index_finalize_pass<3>(policy, gi_output->data(), gi_output->data() + gi_output->real_total_size());
			if (error) return error;

			return gamma_index_output.save(*gi_output);
		}
	}
}