#pragma once

#include <common.hpp>
#include <cmath>
#include <assert.h>
#include <data/image/dose/dose_image_coordinates.hpp>

namespace yagit::core::math
{
	template<typename GenericGIParams, typename ElementType>
	concept GenericGammaIndexParameters = requires(const GenericGIParams & params)
	{
		typename GenericGIParams::value_type;
		typename GenericGIParams::reference;
		typename GenericGIParams::const_reference;
		requires std::same_as<typename GenericGIParams::value_type, ElementType>;
		requires std::same_as<typename GenericGIParams::reference, ElementType&>;
		requires std::same_as<typename GenericGIParams::const_reference, const ElementType&>;
		{params.dose_difference_normalization()}->std::same_as<typename GenericGIParams::const_reference>;
		{params.distance_to_agreement_normalization()}->std::same_as<typename GenericGIParams::const_reference>;
	};

	template<typename GenericGIParams, typename ElementType>
	concept GenericGammaIndexParametersWithSquaredValues =
		GenericGammaIndexParameters<GenericGIParams, ElementType>
		&& requires(const GenericGIParams & params)
	{
		{params.distance_to_agreement_normalization_squared()}->std::same_as<typename GenericGIParams::const_reference>;
	};

	template<typename LocalGIParams, typename ElementType>
	concept LocalGammaIndexParameters = 
		GenericGammaIndexParameters<LocalGIParams, ElementType>
		&& requires(const LocalGIParams& params)
	{
		{params.reference_dose_percentage()}->std::same_as<typename LocalGIParams::const_reference>;
	};

	template<typename LocalGIParams, typename ElementType>
	concept LocalGammaIndexParametersWithSquaredValues =
		LocalGammaIndexParameters<LocalGIParams, ElementType>
		&& GenericGammaIndexParametersWithSquaredValues<LocalGIParams, ElementType>;

	template<typename GlobalGIParams, typename ElementType>
	concept GlobalGammaIndexParameters =
		GenericGammaIndexParameters<GlobalGIParams, ElementType>
		&& requires(const GlobalGIParams& params)
	{
		{params.absolute_dose_difference()}->std::same_as<typename GlobalGIParams::const_reference>;
	};

	template<typename GlobalGIParams, typename ElementType>
	concept GlobalGammaIndexParametersWithSquaredValues =
		GlobalGammaIndexParameters<GlobalGIParams, ElementType>
		&& GenericGammaIndexParametersWithSquaredValues<GlobalGIParams, ElementType>
		&& requires(const GlobalGIParams& params)
	{
		{params.absolute_dose_difference_squared()}->std::same_as<typename GlobalGIParams::const_reference>;
	};

	template<typename GIParamsType, typename ElementType>
	concept LocalOrGlobalGammaIndexParameters =
		LocalGammaIndexParameters<GIParamsType, ElementType>
		|| GlobalGammaIndexParameters<GIParamsType, ElementType>;

	template<typename ElementType, GenericGammaIndexParameters<ElementType> GenericGIParamsType>
	constexpr const ElementType& dose_difference_normalization(const GenericGIParamsType& generic_params)
	{
		return generic_params.dose_difference_normalization();
	}

	template<typename ElementType, GenericGammaIndexParameters<ElementType> GenericGIParamsType>
	constexpr const ElementType& distance_to_agreement_normalization(const GenericGIParamsType& generic_params)
	{
		return generic_params.distance_to_agreement_normalization();
	}

	template<typename ElementType, GenericGammaIndexParameters<ElementType> GenericGIParamsType>
	constexpr ElementType distance_to_agreement_normalization_squared(const GenericGIParamsType& generic_params)
	{
		return distance_to_agreement_normalization(generic_params) * distance_to_agreement_normalization(generic_params);
	}

	template<typename ElementType, GenericGammaIndexParametersWithSquaredValues<ElementType> GenericGIParamsType>
	constexpr const ElementType& distance_to_agreement_normalization_squared(const GenericGIParamsType& generic_params)
	{
		return generic_params.distance_to_agreement_normalization_squared();
	}

	template<typename ElementType, LocalGammaIndexParameters<ElementType> LocalGIParamsType>
	constexpr const ElementType& reference_dose_percentage(const LocalGIParamsType& local_params)
	{
		return local_params.reference_dose_percentage();
	}

	template<typename ElementType, GlobalGammaIndexParameters<ElementType> GlobalGIParamsType>
	constexpr const ElementType& absolute_dose_difference(const GlobalGIParamsType& global_params)
	{
		return global_params.absolute_dose_difference();
	}

	template<typename ElementType, GlobalGammaIndexParameters<ElementType> GlobalGIParamsType>
	constexpr ElementType absolute_dose_difference_squared(const GlobalGIParamsType& global_params)
	{
		return absolute_dose_difference(global_params) * absolute_dose_difference(global_params);
	}

	template<typename ElementType, GlobalGammaIndexParametersWithSquaredValues<ElementType> GlobalGIParamsType>
	constexpr const ElementType& absolute_dose_difference_squared(const GlobalGIParamsType& global_params)
	{
		return global_params.absolute_dose_difference_squared();
	}

	template<typename ElementType>
	class local_gamma_index_params
	{
	public:
		using value_type = ElementType;
		using reference = value_type&;
		using const_reference = const value_type&;
	private:
		value_type _reference_dose_percentage;
		value_type _distance_to_agreement;
		value_type _distance_to_agreement_squared;
	public:
		constexpr local_gamma_index_params(value_type reference_dose_percentage, value_type distance_to_agreement)
			: _reference_dose_percentage(reference_dose_percentage)
			, _distance_to_agreement(distance_to_agreement)
			, _distance_to_agreement_squared(distance_to_agreement* distance_to_agreement)
		{ }
		constexpr local_gamma_index_params(value_type reference_dose_percentage, value_type distance_to_agreement, value_type distance_to_agreement_squared)
			: _reference_dose_percentage(reference_dose_percentage)
			, _distance_to_agreement(distance_to_agreement)
			, _distance_to_agreement_squared(distance_to_agreement_squared)
		{ }
		template<LocalGammaIndexParameters<ElementType> LocalGIParamsType>
		constexpr local_gamma_index_params(const LocalGIParamsType& other_local_params)
			: local_gamma_index_params(
				math::reference_dose_percentage(other_local_params),
				math::distance_to_agreement_normalization(other_local_params)
			)
		{ }
		template<LocalGammaIndexParametersWithSquaredValues<ElementType> LocalGIParamsType>
		constexpr local_gamma_index_params(const LocalGIParamsType& other_local_params)
			: local_gamma_index_params(
				math::reference_dose_percentage(other_local_params),
				math::distance_to_agreement_normalization(other_local_params),
				math::distance_to_agreement_normalization_squared(other_local_params)
			)
		{ }
	public:
		// Dose Difference normalization parameters
		constexpr const_reference dose_difference_normalization() const noexcept { return _reference_dose_percentage; }
		constexpr const_reference reference_dose_percentage() const noexcept { return dose_difference_normalization(); }
		// Distance To Agreement normalization parameters
		constexpr const_reference distance_to_agreement_normalization() const noexcept { return _distance_to_agreement; }
		constexpr const_reference distance_to_agreement_normalization_squared() const noexcept { return _distance_to_agreement_squared; }
	};

	template<typename ElementType, LocalGammaIndexParameters<ElementType> LocalGIParamsType>
	local_gamma_index_params<ElementType> prepare_gi_params(const LocalGIParamsType& local_gi_params)
	{
		return { local_gi_params };
	}

	template<typename ElementType>
	struct global_gamma_index_params
	{
	public:
		using value_type = ElementType;
		using reference = value_type&;
		using const_reference = const value_type&;
	private:
		value_type _absolute_dose_difference;
		value_type _absolute_dose_difference_squared;
		value_type _distance_to_agreement;
		value_type _distance_to_agreement_squared;
	public:
		constexpr global_gamma_index_params(value_type absolute_dose_difference, value_type distance_to_agreement)
			: _absolute_dose_difference(absolute_dose_difference)
			, _absolute_dose_difference_squared(absolute_dose_difference * absolute_dose_difference)
			, _distance_to_agreement(distance_to_agreement)
			, _distance_to_agreement_squared(distance_to_agreement* distance_to_agreement)
		{ }
		constexpr global_gamma_index_params(
			value_type absolute_dose_difference,
			value_type absolute_dose_difference_squared,
			value_type distance_to_agreement,
			value_type distance_to_agreement_squared
		)
			: _absolute_dose_difference(absolute_dose_difference)
			, _absolute_dose_difference_squared(absolute_dose_difference_squared)
			, _distance_to_agreement(distance_to_agreement)
			, _distance_to_agreement_squared(distance_to_agreement_squared)
		{ }
		template<GlobalGammaIndexParameters<ElementType> GlobalGIParamsType>
		constexpr global_gamma_index_params(const GlobalGIParamsType& other_global_params)
			: global_gamma_index_params(
				math::absolute_dose_difference(other_global_params),
				math::distance_to_agreement_normalization(other_global_params)
			)
		{ }
		template<GlobalGammaIndexParametersWithSquaredValues<ElementType> GlobalGIParamsType>
		constexpr global_gamma_index_params(const GlobalGIParamsType& other_global_params)
			: global_gamma_index_params(
				math::absolute_dose_difference(other_global_params),
				math::absolute_dose_difference_squared(other_global_params),
				math::distance_to_agreement_normalization(other_global_params),
				math::distance_to_agreement_normalization_squared(other_global_params)
			)
		{ }
	public:
		// Dose Difference normalization parameters
		constexpr const_reference dose_difference_normalization() const noexcept { return _absolute_dose_difference; }
		constexpr const_reference absolute_dose_difference() const noexcept { return dose_difference_normalization(); }
		constexpr const_reference absolute_dose_difference_squared() const noexcept { return _absolute_dose_difference_squared; }
		// Distance To Agreement normalization parameters
		constexpr const_reference distance_to_agreement_normalization() const noexcept { return _distance_to_agreement; }
		constexpr const_reference distance_to_agreement_normalization_squared() const noexcept { return _distance_to_agreement_squared; }
	};

	template<typename ElementType, GlobalGammaIndexParameters<ElementType> GlobalGIParamsType>
	global_gamma_index_params<ElementType> prepare_gi_params(const GlobalGIParamsType& global_gi_params)
	{
		return { global_gi_params };
	}

	template<typename ElementType, size_t Dimensions>
	struct output_image_view
	{
		data::view<ElementType> output;
		data::sizes<Dimensions> image_size;
	};

	template<typename ElementType, size_t Dimensions>
	struct input_uniform_dose_image_metadata
	{
		data::image_position_t<ElementType, Dimensions> image_position;
		data::uniform_image_spacing_t<ElementType, Dimensions> image_spacing;
	};

	template<typename ElementType, size_t Dimensions>
	struct input_nonuniform_dose_image_metadata
	{
		data::image_position_t<ElementType, Dimensions> image_position;
		data::nonuniform_image_spacing_t<ElementType, Dimensions> image_spacing;
	};

	template<typename ElementType, size_t Dimensions>
	struct input_uniform_image_view
	{
		data::const_view<ElementType> doses;
		data::sizes<Dimensions> image_size;
		input_uniform_dose_image_metadata<ElementType, Dimensions> image_metadata;
	};

	template<typename ElementType, size_t Dimensions>
	struct input_nonuniform_image_view
	{
		data::const_view<ElementType> doses;
		data::sizes<Dimensions> image_size;
		input_uniform_dose_image_metadata<ElementType, Dimensions> image_metadata;
	};

	template<typename ImageViewType, typename ElementType, size_t Dimensions>
	concept AnyInputImageView =
		std::is_same_v<ImageViewType, input_uniform_image_view<ElementType, Dimensions>>
		|| std::is_same_v<ImageViewType, input_nonuniform_image_view<ElementType, Dimensions>>;

	namespace algorithm_version
	{
		namespace old
		{
			struct classic {};
			struct classic_with_rectangle {};
			struct spiral {};
			struct spiral_with_rectangle {};
		}
		/// <summary>
		/// Performs all to all comparison - every reference point to every target point
		/// Skips NaN elements
		/// </summary>
		struct classic 
		{
			template<typename ElementType, size_t Dimensions>
			struct parameters 
			{
				/// <summary>
				/// Views to reference image coordinates along each dimensions.
				/// Coordinates must be ordered the same way as reference image doses
				/// </summary>
				array<data::const_view<ElementType>, Dimensions> reference_image_coordinates;
				/// <summary>
				/// Views to target image coordinates along each dimensions.
				/// Coordinates must be ordered the same way as target image doses
				/// </summary>
				array<data::const_view<ElementType>, Dimensions> target_image_coordinates;
			};
		};
		/// <summary>
		/// Performs calculation for each reference point starting with projecting reference point
		/// onto target image space and traverses target image using steps up to a max_distance
		/// between reference point coordinate and currently evaluated point
		/// Uses linear interpolation for approximation of fractional grid points.
		/// Skips NaN elements
		/// </summary>
		struct grid_search 
		{
			template<typename ElementType, size_t Dimensions>
			struct parameters
			{
				/// <summary>
				/// Steps size along each axii in target space coordinate traversal.
				/// Note: Lowering these values may cause longer calculation times
				/// </summary>
				array<ElementType, 3> step;
				/// <summary>
				/// Maximum distance between currently evaluated reference point and target image point.
				/// Exceeding this value causes algorithm to continue with evaluation of next reference image point.
				/// Setting this value to distance_to_agreement value will effectively cause the algorithm to search for
				/// points that only satisfy gamma index test
				/// Note: Increasing this value may cause longer calculation times
				/// </summary>
				ElementType max_distance;
			};
		};
	}

	namespace execution
	{
		using sequenced_policy = std::execution::sequenced_policy;
		using parallel_policy = std::execution::parallel_policy;
		using unsequenced_policy = std::execution::unsequenced_policy;
		using parallel_unsequenced_policy = std::execution::parallel_unsequenced_policy;
		/// <summary>
		/// Special execution policy indicating that user wants to execute algorithm
		/// on an offloading device (eg. GPU)
		/// </summary>
		struct offload_policy
		{

		};
	}

	/// <summary>
	/// Extension to std::transform that allows using more than 2 input iterators
	/// </summary>
	/// <returns></returns>
	template<typename ExecutionPolicy, typename InIt0, typename OutIt, typename Op, typename... InIts>
	constexpr OutIt transform(ExecutionPolicy&& policy, InIt0 first0, InIt0 last0, OutIt dfirst, Op op, InIts... firsts)
	{
		while (first0 != last0)
		{
			*dfirst++ = op(*first0++, *firsts++...);
		}
		return dfirst;
	}

	/// <summary>
	/// Extension to std::transform that allows using more than 2 input iterators.
	/// Specialization for parallel_policy
	/// </summary>
	/// <returns></returns>
	template<typename InIt0, typename OutIt, typename Op, typename... InIts>
	constexpr OutIt transform(execution::parallel_policy parallel_policy_params, InIt0 first0, InIt0 last0, OutIt dfirst, Op op, InIts... firsts)
	{
#if defined(YAGIT_OPENMP)
		auto do_advance = [](auto it, auto i)
		{
			std::advance(it, i);
			return it;
		};

		using ptrdiff_t = typename std::iterator_traits<InIt0>::difference_type;
		ptrdiff_t count = last0 - first0;

		for (ptrdiff_t i = static_cast<ptrdiff_t>(0); i < count; ++i)
		{
			*do_advance(dfirst, i) = op(*do_advance(first0, i), *do_advance(firsts, i)...);
		}

		return do_advance(dfirst, count);
#else
		transform(execution::sequenced_policy{}, first0, last0, dfirst, op, firsts...);
#endif
	}
}