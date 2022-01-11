#pragma once

#include <common.hpp>
#include <data/image/dose/dose_image_coordinates.hpp>
#include <random>
#include <iostream>

namespace yagit::additions
{
	namespace detail
	{
		template<typename ElementType>
		struct nan_aware_min_less
			: std::less<ElementType>
		{
			constexpr bool operator()(const ElementType& left, const ElementType& right) const
			{
				if (std::isnan(right) && !std::isnan(left))
					return true;
				return std::less<ElementType>::operator()(left, right);
			}
		};

		template<typename ElementType>
		struct nan_aware_max_less
			: std::less<ElementType>
		{
			constexpr bool operator()(const ElementType& left, const ElementType& right) const
			{
				if (std::isnan(left) && !std::isnan(right))
					return true;
				return std::less<ElementType>::operator()(left, right);
			}
		};

		template<typename ElementType>
		ElementType nan_to(ElementType v, ElementType replacement_if_nan)
		{
			return std::isnan(v) ? replacement_if_nan : v;
		}
	}

	template<typename ElementType, typename F = detail::nan_aware_min_less<ElementType>>
	ElementType min(core::data::const_view<ElementType> image_data, F compare = detail::nan_aware_min_less<ElementType>())
	{
		return *std::min_element(std::execution::par_unseq, std::begin(image_data), std::end(image_data), compare);
	}

	template<typename ElementType, typename F = detail::nan_aware_max_less<ElementType>>
	ElementType max(core::data::const_view<ElementType> image_data, F compare = detail::nan_aware_max_less<ElementType>())
	{
		return *std::max_element(std::execution::par_unseq, std::begin(image_data), std::end(image_data), compare);
	}

	template<typename ElementType, typename F = detail::nan_aware_max_less<ElementType>>
	ElementType avg(core::data::const_view<ElementType> image_data)
	{
		auto total = std::accumulate(std::begin(image_data), std::end(image_data), static_cast<ElementType>(0), [](auto&& a, auto&& b) 
			{
				return detail::nan_to(a, static_cast<ElementType>(0)) + detail::nan_to(b, static_cast<ElementType>(0));
			});
		return total / std::size(image_data);
	}

	template<typename ElementType, typename F = detail::nan_aware_max_less<ElementType>>
	ElementType avg_abs_gi_difference(core::data::const_view<ElementType> image_data_ref, core::data::const_view<ElementType> image_data_tar)
	{
		auto total = std::transform_reduce(
			std::execution::par_unseq,
			std::begin(image_data_ref), std::end(image_data_ref),
			std::begin(image_data_tar), static_cast<ElementType>(0),
			std::plus<ElementType>(),
			[](auto&& a, auto&& b) {return std::abs(detail::nan_to(a, static_cast<ElementType>(0)) - detail::nan_to(b, static_cast<ElementType>(0))); }
		);
		return total / std::size(image_data_ref);
	}

	template<typename ElementType, typename F>
	void filter_image_data(core::data::view<ElementType> image_data, F f)
	{
		auto nan = static_cast<ElementType>(std::nan(""));
		std::transform(std::execution::par_unseq, std::begin(image_data), std::end(image_data), std::begin(image_data), [nan, &f](auto&& v)
			{
				return f(v) ? nan : v;
			}
		);
	}

	template<typename ElementType>
	void filter_image_data_based_on_dose_cutoff(core::data::view<ElementType> image_data, ElementType dose_cutoff)
	{
		filter_image_data(image_data, [dose_cutoff](auto&& v) { return v < dose_cutoff; });
	}

	template<typename ElementType>
	void filter_image_data_noise(core::data::view<ElementType> image_data, ElementType noise_percentage_limit)
	{
		auto image_min = min(image_data);
		auto image_max = max(image_data);
		auto threshold = image_min + noise_percentage_limit * (image_max - image_min);
		filter_image_data(image_data, [threshold](auto&& v) { return v >= threshold; });
	}

	template<typename ElementType, size_t Dimensions>
	void scale_and_bias_image_data(core::data::view<ElementType> image_data, ElementType scale, ElementType bias = static_cast<ElementType>(0))
	{
		std::transform(std::execution::par_unseq, std::begin(image_data), std::end(image_data), std::begin(image_data), [scale, bias](auto&& v)
			{
				return v * scale + bias;
			}
		);
	}

	template<typename ElementType, typename F>
	size_t voxels_satifying_predicate(core::data::const_view<ElementType> image_data, F predicate)
	{
		return std::count_if(std::execution::par_unseq, std::begin(image_data), std::end(image_data), predicate);
	}

	template<typename ElementType>
	size_t active_voxels_count(core::data::const_view<ElementType> image_data)
	{
		return voxels_satifying_predicate(image_data, [](auto&& v) {return !std::isnan(v); });
	}

	template<typename ElementType>
	size_t passing_voxels_count(core::data::const_view<ElementType> image_data)
	{
		return voxels_satifying_predicate(image_data, [](auto&& v) {return v >= static_cast<ElementType>(0) && v <= static_cast<ElementType>(1); });
	}

	template<typename ElementType>
	ElementType active_voxels_percentage(core::data::const_view<ElementType> image_data)
	{
		auto active_count = active_voxels_count(image_data);
		return static_cast<ElementType>(active_count) / std::size(image_data);
	}

	template<typename ElementType>
	ElementType active_voxels_passing_rate(core::data::const_view<ElementType> image_data)
	{
		auto active_count = active_voxels_count(image_data);
		auto passing_count = passing_voxels_count(image_data);
		return static_cast<ElementType>(passing_count) / active_count;
	}

	template<typename ElementType>
	ElementType total_voxels_passing_rate(core::data::const_view<ElementType> image_data)
	{
		auto passing_count = passing_voxels_count(image_data);
		return static_cast<ElementType>(passing_count) / std::size(image_data);
	}

	/// <summary>
	/// delimeters = [delimeter_0, delimeter_1,...,delimeter_N]
	/// histogram_output = [<delim_0-delim_1), <delim_1, delim_2), ..., <delim_N-1, delim_N), (not in delims), (optional)(NaNs)]
	/// </summary>
	template<bool IncludeNaN, typename ElementType>
	void make_histogram(
		size_t* histogram_output,
		core::data::const_view<ElementType> image_data,
		const ElementType* delimiters, const ElementType* delimiters_end)
	{
		auto count_bins = (delimiters_end - delimiters) - 1; // last bin is for voxels not within delimeters;
		auto nan_it = histogram_output + count_bins + 1; // if IncludeNaN there is bin after last bin for NaN voxel values
		std::fill(std::execution::par_unseq, histogram_output, histogram_output + count_bins + 1 + static_cast<size_t>(IncludeNaN), static_cast<size_t>(0));
		std::for_each(std::execution::seq, std::begin(image_data), std::end(image_data), [histogram_output, delimiters, delimiters_end, count_bins, nan_it](auto&& v)
			{
				if (std::isnan(v))
				{
					if constexpr (IncludeNaN)
					{
						(*nan_it)++;
					}
				}
				else
				{
					auto bin_it = std::upper_bound(delimiters, delimiters_end, v);
					if (bin_it == delimiters_end || bin_it == delimiters)
					{
						histogram_output[count_bins]++;
					}
					else
					{
						auto bin_index = (bin_it - delimiters) - 1;
						histogram_output[bin_index]++;
					}
				}
			}
		);
	}

	/// <summary>
	/// delimeters = [delimeter_0, delimeter_1,...,delimeter_N]
	/// histogram_output = [<delim_0-delim_1), <delim_1, delim_2), ..., <delim_N-1, delim_N), (not in delims), (optional)(NaNs)]
	/// </summary>
	template<bool IncludeNaN, typename ElementType>
	void make_cumulative_histogram(
		size_t* histogram_output,
		core::data::const_view<ElementType> image_data,
		const ElementType* delimiters, const ElementType* delimiters_end)
	{
		make_histogram(histogram_output, image_data, delimiters, delimiters_end);
		auto count_delim_bins = (delimiters_end - delimiters) - 1;
		// only cumulate bins that represent values that have found their delimeter ranges
		std::partial_sum(histogram_output, histogram_output + count_delim_bins, histogram_output, std::plus<size_t>());
	}

	template<typename ElementType>
	void generate_random_doses(core::data::view<ElementType> image_dose_data, ElementType low, ElementType high)
	{
		static random_device r;
		static default_random_engine e1(r());
		uniform_real_distribution<ElementType> uniform_dist(low, high);
		auto gen = [&]() {return uniform_dist(e1); };
		std::generate(std::begin(image_dose_data), std::end(image_dose_data), gen);
	}

	namespace detail
	{
		template<size_t Dimensions>
		constexpr size_t flatten(const std::array<size_t, Dimensions>& index, const yagit::core::data::sizes<Dimensions>& image_size)
		{
			size_t flattened_index = 0;
			for (size_t i = 0; i < Dimensions; i++)
			{
				flattened_index *= image_size[i];
				flattened_index += index[i];
			}
			return flattened_index;
		}

		template<typename ElementType, size_t Dimensions, size_t I0, size_t... I, size_t... IA>
		constexpr void generate_coordinates_impl(
			const std::array<core::data::view<ElementType>, Dimensions> image_coordinates_data,
			const yagit::core::data::sizes<Dimensions>& image_size,
			const yagit::core::data::image_position_t<ElementType, Dimensions>& image_position,
			const yagit::core::data::uniform_image_spacing_t<ElementType, Dimensions>& image_spacing,
			std::array<size_t, Dimensions>& index,
			std::array<ElementType, Dimensions>& coord,
			std::index_sequence<I0, I...>,
			std::index_sequence<IA...> _i
			)
		{
			if constexpr (sizeof...(I) == 0)
			{
				for (index[I0] = 0; index[I0] < image_size[I0]; ++index[I0])
				{
					coord[I0] = image_position[I0] + image_spacing[I0] * index[I0];
					auto flattened_index = flatten(index, image_size);
					auto dummy = {
						(image_coordinates_data[IA][flattened_index] = coord[IA], 0)...
					};
				}
			}
			else
			{
				for (index[I0] = 0; index[I0] < image_size[I0]; ++index[I0])
				{
					coord[I0] = image_position[I0] + image_spacing[I0] * index[I0];
					generate_coordinates_impl(
						image_coordinates_data, image_size,
						image_position, image_spacing,
						index, coord,
						std::index_sequence<I...>{},
						_i
					);
				}
					
			}
		}

		template<typename ElementType, size_t Dimensions>
		constexpr void generate_coordinates_impl(
			const std::array<core::data::view<ElementType>, Dimensions> image_coordinates_data,
			const yagit::core::data::sizes<Dimensions>& image_size,
			const yagit::core::data::image_position_t<ElementType, Dimensions>& image_position,
			const yagit::core::data::uniform_image_spacing_t<ElementType, Dimensions>& image_spacing
		)
		{
			std::array<size_t, Dimensions> index;
			std::array<ElementType, Dimensions> coord;
			generate_coordinates_impl(
				image_coordinates_data, image_size,
				image_position, image_spacing,
				index, coord,
				std::make_index_sequence<Dimensions>(),
				std::make_index_sequence<Dimensions>()
			);
		}

		template<typename ElementType, size_t Dimensions, size_t I0, size_t... I, size_t... IA>
		constexpr void generate_coordinates_impl(
			const std::array<core::data::view<ElementType>, Dimensions> image_coordinates_data,
			const yagit::core::data::sizes<Dimensions>& image_size,
			const yagit::core::data::image_position_t<ElementType, Dimensions>& image_position,
			const yagit::core::data::nonuniform_image_spacing_t<ElementType, Dimensions>& image_spacing,
			std::array<size_t, Dimensions>& index,
			std::array<ElementType, Dimensions>& coord,
			std::index_sequence<I0, I...>,
			std::index_sequence<IA...> _i
		)
		{
			if constexpr (sizeof...(I) == 0)
			{
				coord[I0] = image_position[I0];
				for (index[I0] = 0; index[I0] < image_size[I0]; ++index[I0])
				{
					auto flattened_index = flatten(index, image_size);
					if (flattened_index >= yagit::core::data::total_size(image_size))
						std::cout << "outside bounds" << std::endl;
					auto dummy = {
						(image_coordinates_data[IA][flattened_index] = coord[IA], 0)...
					};
					if (index[I0] <= image_size[I0])
						coord[I0] += image_spacing[I0][index[I0]];
				}
			}
			else
			{
				coord[I0] = image_position[I0];
				for (index[I0] = 0; index[I0] < image_size[I0]; ++index[I0])
				{
					generate_coordinates_impl(
						image_coordinates_data, image_size,
						image_position, image_spacing,
						index, coord,
						std::index_sequence<I...>{},
						_i
					);

					if (index[I0] <= image_size[I0])
						coord[I0] += image_spacing[I0][index[I0]];
				}

			}
		}

		template<typename ElementType, size_t Dimensions>
		constexpr void generate_coordinates_impl(
			const std::array<core::data::view<ElementType>, Dimensions> image_coordinates_data,
			const yagit::core::data::sizes<Dimensions>& image_size,
			const yagit::core::data::image_position_t<ElementType, Dimensions>& image_position,
			const yagit::core::data::nonuniform_image_spacing_t<ElementType, Dimensions>& image_spacing
		)
		{
			std::array<size_t, Dimensions> index;
			std::array<ElementType, Dimensions> coord;
			generate_coordinates_impl(
				image_coordinates_data, image_size,
				image_position, image_spacing,
				index, coord,
				std::make_index_sequence<Dimensions>(),
				std::make_index_sequence<Dimensions>()
			);
		}
	}
	
	template<typename ElementType, size_t Dimensions>
	void generate_coordinates(
		const std::array<core::data::view<ElementType>, Dimensions> image_coordinates_data,
		const yagit::core::data::sizes<Dimensions>& image_size,
		const yagit::core::data::image_position_t<ElementType, Dimensions>& image_position,
		const yagit::core::data::uniform_image_spacing_t<ElementType, Dimensions>& image_spacing
	)
	{
		detail::generate_coordinates_impl(image_coordinates_data, image_size, image_position, image_spacing);
	}

	template<typename ElementType, size_t Dimensions>
	void generate_coordinates(
		const std::array<core::data::view<ElementType>, Dimensions> image_coordinates_data,
		const yagit::core::data::sizes<Dimensions>& image_size,
		const yagit::core::data::image_position_t<ElementType, Dimensions>& image_position,
		const yagit::core::data::nonuniform_image_spacing_t<ElementType, Dimensions>& image_spacing
	)
	{
		detail::generate_coordinates_impl(image_coordinates_data, image_size, image_position, image_spacing);
	}
}