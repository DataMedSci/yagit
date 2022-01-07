#pragma once

#include <data/image/image_data.hpp>

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
	}

	template<typename ElementType, typename F = detail::nan_aware_min_less<ElementType>>
	ElementType min(core::view<ElementType> image_data, core::view<ElementType> image_data_end, F compare = detail::nan_aware_min_less<ElementType>())
	{
		return *std::min_element(std::execution::par_unseq, image_data, image_data_end, compare);
	}

	template<typename ElementType, typename F = detail::nan_aware_max_less<ElementType>>
	ElementType max(core::view<ElementType> image_data, core::view<ElementType> image_data_end, F compare = detail::nan_aware_max_less<ElementType>())
	{
		return *std::max_element(std::execution::par_unseq, image_data, image_data_end, compare);
	}

	template<typename ElementType, typename F>
	void filter_image_data(core::view<ElementType> image_data, core::view<ElementType> image_data_end, F f)
	{
		auto nan = static_cast<ElementType>(std::nan(""));
		std::transform(std::execution::par_unseq, image_data, image_data_end, image_data, [nan, &f](auto&& v)
			{
				return f(v) ? nan : v;
			}
		);
	}

	template<typename ElementType>
	void filter_image_data_based_on_dose_cutoff(core::view<ElementType> image_data, core::view<ElementType> image_data_end, ElementType dose_cutoff)
	{
		filter_image_data(image_data, image_data_end, [dose_cutoff](auto&& v) { return v < dose_cutoff; });
	}

	template<typename ElementType>
	void filter_image_data_noise(core::view<ElementType> image_data, core::view<ElementType> image_data_end, ElementType noise_percentage_limit)
	{
		auto image_min = min(image_data, image_data_end);
		auto image_max = max(image_data, image_data_end);
		auto threshold = image_min + noise_percentage_limit * (image_max - image_min);
		filter_image_data(image_data, image_data_end, [threshold](auto&& v) { return v >= threshold; });
	}

	template<typename ElementType, size_t Dimensions>
	void scale_and_bias_image_data(core::view<ElementType> image_data, core::view<ElementType> image_data_end, ElementType scale, ElementType bias = static_cast<ElementType>(0))
	{
		std::transform(std::execution::par_unseq, image_data, image_data_end, image_data, [scale, bias](auto&& v)
			{
				return v * scale + bias;
			}
		);
	}

	template<typename ElementType, typename F>
	size_t voxels_satifying_predicate(core::view<ElementType> image_data, core::view<ElementType> image_data_end, F predicate)
	{
		return std::count_if(std::execution::par_unseq, image_data, image_data_end, predicate);
	}

	template<typename ElementType>
	size_t active_voxels_count(core::view<ElementType> image_data, core::view<ElementType> image_data_end)
	{
		return voxels_satifying_predicate(image_data, image_data_end, [](auto&& v) {return !std::isnan(v); });
	}

	template<typename ElementType>
	size_t passing_voxels_count(core::view<ElementType> image_data, core::view<ElementType> image_data_end)
	{
		return voxels_satifying_predicate(image_data, image_data_end, [](auto&& v) {return v >= static_cast<ElementType>(0) && v <= static_cast<ElementType>(1); });
	}

	template<typename ElementType>
	ElementType active_voxels_percentage(core::view<ElementType> image_data, core::view<ElementType> image_data_end)
	{
		auto active_count = active_voxels_count(image_data, image_data_end);
		auto total_count = image_data_end - image_data;
		return static_cast<ElementType>(active_count) / total_count;
	}

	template<typename ElementType>
	ElementType active_voxels_passing_rate(core::view<ElementType> image_data, core::view<ElementType> image_data_end)
	{
		auto active_count = active_voxels_count(image_data, image_data_end);
		auto passing_count = passing_voxels_count(image_data, image_data_end);
		return static_cast<ElementType>(passing_count) / active_count;
	}

	template<typename ElementType>
	ElementType total_voxels_passing_rate(core::view<ElementType> image_data, core::view<ElementType> image_data_end)
	{
		auto total_count = image_data_end - image_data;
		auto passing_count = passing_voxels_count(image_data, image_data_end);
		return static_cast<ElementType>(passing_count) / total_count;
	}

	/// <summary>
	/// delimeters = [delimeter_0, delimeter_1,...,delimeter_N]
	/// histogram_output = [<delim_0-delim_1), <delim_1, delim_2), ..., <delim_N-1, delim_N), (not in delims), (optional)(NaNs)]
	/// </summary>
	template<bool IncludeNaN, typename ElementType>
	void make_histogram(
		core::view<size_t> histogram_output,
		core::view<ElementType> image_data, core::view<ElementType> image_data_end,
		core::const_view<ElementType> delimiters, core::const_view<ElementType> delimiters_end)
	{
		auto count_bins = (delimiters_end - delimiters) - 1; // last bin is for voxels not within delimeters;
		auto nan_it = histogram_output + count_bins + 1; // if IncludeNaN there is bin after last bin for NaN voxel values
		std::fill(std::execution::par_unseq, histogram_output, histogram_output + count_bins + 1 + static_cast<size_t>(IncludeNaN), static_cast<size_t>(0));
		std::for_each(std::execution::seq, image_data, image_data_end, [histogram_output, delimiters, delimiters_end, count_bins, nan_it](auto&& v)
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
		core::view<size_t> histogram_output,
		core::view<ElementType> image_data, core::view<ElementType> image_data_end,
		core::const_view<ElementType> delimiters, core::const_view<ElementType> delimiters_end)
	{
		make_histogram(histogram_output, image_data, image_data_end, delimiters, delimiters_end);
		auto count_delim_bins = (delimiters_end - delimiters) - 1;
		// only cumulate bins that represent values that have found their delimeter ranges
		std::partial_sum(histogram_output, histogram_output + count_delim_bins, histogram_output, std::plus<size_t>());
	}
}