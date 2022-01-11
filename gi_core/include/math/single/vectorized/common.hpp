#pragma once

#include <math/common.hpp>

#include <simdpp/simd.h>

namespace yagit::core::math::vectorized
{
	template<typename ElementType, unsigned VectorSize>
	struct vec {};

	template<unsigned VectorSize>
	struct vec<int8_t, VectorSize> { using type = simdpp::int8<VectorSize>; constexpr static unsigned vector_size = VectorSize; };
	template<unsigned VectorSize>
	struct vec<int16_t, VectorSize> { using type = simdpp::int16<VectorSize>; constexpr static unsigned vector_size = VectorSize; };
	template<unsigned VectorSize>
	struct vec<int32_t, VectorSize> { using type = simdpp::int32<VectorSize>; constexpr static unsigned vector_size = VectorSize; };
	template<unsigned VectorSize>
	struct vec<int64_t, VectorSize> { using type = simdpp::int64<VectorSize>; constexpr static unsigned vector_size = VectorSize; };

	template<unsigned VectorSize>
	struct vec<uint8_t, VectorSize> { using type = simdpp::uint8<VectorSize>; constexpr static unsigned vector_size = VectorSize; };
	template<unsigned VectorSize>
	struct vec<uint16_t, VectorSize> { using type = simdpp::uint16<VectorSize>; constexpr static unsigned vector_size = VectorSize; };
	template<unsigned VectorSize>
	struct vec<uint32_t, VectorSize> { using type = simdpp::uint32<VectorSize>; constexpr static unsigned vector_size = VectorSize; };
	template<unsigned VectorSize>
	struct vec<uint64_t, VectorSize> { using type = simdpp::uint64<VectorSize>; constexpr static unsigned vector_size = VectorSize; };

	template<unsigned VectorSize>
	struct vec<float, VectorSize> { using type = simdpp::float32<VectorSize>; constexpr static unsigned vector_size = VectorSize; };
	template<unsigned VectorSize>
	struct vec<double, VectorSize> { using type = simdpp::float64<VectorSize>; constexpr static unsigned vector_size = VectorSize; };

	template<typename ElementType, unsigned VectorSize>
	using vec_t = typename vec<ElementType, VectorSize>::type;
	template<typename ElementType, unsigned VectorSize>
	using vec_size_v = vec<ElementType, VectorSize>::vector_size;

	template<typename GenericGIParams, typename ElementType, size_t VectorSize>
	concept GenericVectorGammaIndexParameters = requires(const GenericGIParams& params)
	{
		typename GenericGIParams::value_type;
		typename GenericGIParams::reference;
		typename GenericGIParams::const_reference;
		requires std::same_as<typename GenericGIParams::value_type, vec_t<ElementType, VectorSize>>;
		requires std::same_as<typename GenericGIParams::reference, vec_t<ElementType, VectorSize>&>;
		requires std::same_as<typename GenericGIParams::const_reference, const vec_t<ElementType, VectorSize>&>;
		{params.dose_difference_normalization()}->std::same_as<typename GenericGIParams::const_reference>;
		{params.distance_to_agreement_normalization()}->std::same_as<typename GenericGIParams::const_reference>;
	};

	template<typename GenericGIParams, typename ElementType, size_t VectorSize>
	concept GenericVectorGammaIndexParametersWithSquaredValues =
		GenericVectorGammaIndexParameters<GenericGIParams, ElementType, VectorSize>
		&& requires(const GenericGIParams& params)
	{
		{params.distance_to_agreement_normalization_squared()}->std::same_as<typename GenericGIParams::const_reference>;
	};

	template<typename LocalGIParams, typename ElementType, size_t VectorSize>
	concept LocalVectorGammaIndexParameters =
		GenericVectorGammaIndexParameters<LocalGIParams, ElementType, VectorSize>
		&& requires(const LocalGIParams& params)
	{
		{params.reference_dose_percentage()}->std::same_as<typename LocalGIParams::const_reference>;
	};

	template<typename LocalGIParams, typename ElementType, size_t VectorSize>
	concept LocalVectorGammaIndexParametersWithSquaredValues =
		LocalVectorGammaIndexParameters<LocalGIParams, ElementType, VectorSize>
		&& GenericVectorGammaIndexParametersWithSquaredValues<LocalGIParams, ElementType, VectorSize>;

	template<typename GlobalGIParams, typename ElementType, size_t VectorSize>
	concept GlobalVectorGammaIndexParameters =
		GenericVectorGammaIndexParameters<GlobalGIParams, ElementType, VectorSize>
		&& requires(const GlobalGIParams& params)
	{
		{params.absolute_dose_difference()}->std::same_as<typename GlobalGIParams::const_reference>;
	};

	template<typename GlobalGIParams, typename ElementType, size_t VectorSize>
	concept GlobalVectorGammaIndexParametersWithSquaredValues =
		GlobalVectorGammaIndexParameters<GlobalGIParams, ElementType, VectorSize>
		&& GenericVectorGammaIndexParametersWithSquaredValues<GlobalGIParams, ElementType, VectorSize>
		&& requires(const GlobalGIParams& params)
	{
		{params.absolute_dose_difference_squared()}->std::same_as<typename GlobalGIParams::const_reference>;
	};

	template<typename GIParamsType, typename ElementType, size_t VectorSize>
	concept LocalOrGlobalVectorGammaIndexParameters =
		LocalVectorGammaIndexParameters<GIParamsType, ElementType, VectorSize>
		|| GlobalVectorGammaIndexParameters<GIParamsType, ElementType, VectorSize>;

	template<typename ElementType, size_t VectorSize, GenericVectorGammaIndexParameters<ElementType, VectorSize> GenericGIParamsType>
	constexpr const vec_t<ElementType, VectorSize>& dose_difference_normalization(const GenericGIParamsType& generic_params)
	{
		return generic_params.dose_difference_normalization();
	}

	template<typename ElementType, size_t VectorSize, GenericVectorGammaIndexParameters<ElementType, VectorSize> GenericGIParamsType>
	constexpr const vec_t<ElementType, VectorSize>& distance_to_agreement_normalization(const GenericGIParamsType& generic_params)
	{
		return generic_params.distance_to_agreement_normalization();
	}

	template<typename ElementType, size_t VectorSize, GenericVectorGammaIndexParameters<ElementType, VectorSize> GenericGIParamsType>
	constexpr vec_t<ElementType, VectorSize> distance_to_agreement_normalization_squared(const GenericGIParamsType& generic_params)
	{
		return distance_to_agreement_normalization(generic_params) * distance_to_agreement_normalization(generic_params);
	}

	template<typename ElementType, size_t VectorSize, GenericVectorGammaIndexParametersWithSquaredValues<ElementType, VectorSize> GenericGIParamsType>
	constexpr const vec_t<ElementType, VectorSize>& distance_to_agreement_normalization_squared(const GenericGIParamsType& generic_params)
	{
		return generic_params.distance_to_agreement_normalization_squared();
	}

	template<typename ElementType, size_t VectorSize, LocalVectorGammaIndexParameters<ElementType, VectorSize> LocalGIParamsType>
	constexpr const vec_t<ElementType, VectorSize>& reference_dose_percentage(const LocalGIParamsType& local_params)
	{
		return local_params.reference_dose_percentage();
	}

	template<typename ElementType, size_t VectorSize, GlobalVectorGammaIndexParameters<ElementType, VectorSize> GlobalGIParamsType>
	constexpr const vec_t<ElementType, VectorSize>& absolute_dose_difference(const GlobalGIParamsType& global_params)
	{
		return global_params.absolute_dose_difference();
	}

	template<typename ElementType, size_t VectorSize, GlobalVectorGammaIndexParameters<ElementType, VectorSize> GlobalGIParamsType>
	constexpr vec_t<ElementType, VectorSize> absolute_dose_difference_squared(const GlobalGIParamsType& global_params)
	{
		return absolute_dose_difference(global_params) * absolute_dose_difference(global_params);
	}

	template<typename ElementType, size_t VectorSize, GlobalVectorGammaIndexParametersWithSquaredValues<ElementType, VectorSize> GlobalGIParamsType>
	constexpr const vec_t<ElementType, VectorSize>& absolute_dose_difference_squared(const GlobalGIParamsType& global_params)
	{
		return global_params.absolute_dose_difference_squared();
	}

	template<typename ElementType, size_t VectorSize>
	class vec_local_gamma_index_params
	{
	public:
		using value_type = vec_t<ElementType, VectorSize>;
		using reference = value_type&;
		using const_reference = const value_type&;
	private:
		value_type _reference_dose_percentage;
		value_type _distance_to_agreement;
		value_type _distance_to_agreement_squared;
	public:
		constexpr vec_local_gamma_index_params(value_type reference_dose_percentage, value_type distance_to_agreement)
			: _reference_dose_percentage(reference_dose_percentage)
			, _distance_to_agreement(distance_to_agreement)
			, _distance_to_agreement_squared(distance_to_agreement* distance_to_agreement)
		{ }
		constexpr vec_local_gamma_index_params(value_type reference_dose_percentage, value_type distance_to_agreement, value_type distance_to_agreement_squared)
			: _reference_dose_percentage(reference_dose_percentage)
			, _distance_to_agreement(distance_to_agreement)
			, _distance_to_agreement_squared(distance_to_agreement_squared)
		{ }
		template<LocalGammaIndexParameters<ElementType> LocalGIParamsType>
		constexpr vec_local_gamma_index_params(const LocalGIParamsType& other_local_params)
			: vec_local_gamma_index_params(
				simdpp::splat(math::reference_dose_percentage(other_local_params)),
				simdpp::splat(math::distance_to_agreement_normalization(other_local_params))
			)
		{ }
		template<LocalGammaIndexParametersWithSquaredValues<ElementType> LocalGIParamsType>
		constexpr vec_local_gamma_index_params(const LocalGIParamsType& other_local_params)
			: vec_local_gamma_index_params(
				simdpp::splat(math::reference_dose_percentage<ElementType>(other_local_params)),
				simdpp::splat(math::distance_to_agreement_normalization<ElementType>(other_local_params)),
				simdpp::splat(math::distance_to_agreement_normalization_squared<ElementType>(other_local_params))
			)
		{ }

		template<LocalVectorGammaIndexParameters<ElementType> LocalGIParamsType>
		constexpr vec_local_gamma_index_params(const LocalGIParamsType& other_local_params)
			: vec_local_gamma_index_params(
				math::vectorized::reference_dose_percentage(other_local_params),
				math::vectorized::distance_to_agreement_normalization(other_local_params)
			)
		{ }
		template<LocalVectorGammaIndexParametersWithSquaredValues<ElementType> LocalGIParamsType>
		constexpr vec_local_gamma_index_params(const LocalGIParamsType& other_local_params)
			: vec_local_gamma_index_params(
				math::vectorized::reference_dose_percentage(other_local_params),
				math::vectorized::distance_to_agreement_normalization(other_local_params),
				math::vectorized::distance_to_agreement_normalization_squared(other_local_params)
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

	template<size_t VectorSize, typename ElementType, LocalGammaIndexParameters<ElementType> LocalGIParamsType>
	vec_local_gamma_index_params<ElementType, VectorSize> prepare_vectorized_gi_params(const LocalGIParamsType& local_gi_params)
	{
		return { local_gi_params };
	}

	template<size_t VectorSize, typename ElementType, LocalVectorGammaIndexParameters<ElementType, VectorSize> LocalGIParamsType>
	vec_local_gamma_index_params<ElementType, VectorSize> prepare_vectorized_gi_params(const LocalGIParamsType& local_gi_params)
	{
		return { local_gi_params };
	}

	template<typename ElementType, size_t VectorSize>
	class vec_global_gamma_index_params
	{
	public:
		using value_type = vec_t<ElementType, VectorSize>;
		using reference = value_type&;
		using const_reference = const value_type&;
	private:
		value_type _absolute_dose_difference;
		value_type _absolute_dose_difference_squared;
		value_type _distance_to_agreement;
		value_type _distance_to_agreement_squared;
	public:
		constexpr vec_global_gamma_index_params(value_type absolute_dose_difference, value_type distance_to_agreement)
			: _absolute_dose_difference(absolute_dose_difference)
			, _absolute_dose_difference_squared(absolute_dose_difference* absolute_dose_difference)
			, _distance_to_agreement(distance_to_agreement)
			, _distance_to_agreement_squared(distance_to_agreement* distance_to_agreement)
		{ }
		constexpr vec_global_gamma_index_params(
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
		constexpr vec_global_gamma_index_params(const GlobalGIParamsType& other_global_params)
			: vec_global_gamma_index_params(
				simdpp::splat(math::absolute_dose_difference(other_global_params)),
				simdpp::splat(math::distance_to_agreement_normalization(other_global_params))
			)
		{ }
		template<GlobalGammaIndexParametersWithSquaredValues<ElementType> GlobalGIParamsType>
		constexpr vec_global_gamma_index_params(const GlobalGIParamsType& other_global_params)
			: vec_global_gamma_index_params(
				simdpp::splat(math::absolute_dose_difference<ElementType>(other_global_params)),
				simdpp::splat(math::absolute_dose_difference_squared<ElementType>(other_global_params)),
				simdpp::splat(math::distance_to_agreement_normalization<ElementType>(other_global_params)),
				simdpp::splat(math::distance_to_agreement_normalization_squared<ElementType>(other_global_params))
			)
		{ }
		template<GlobalVectorGammaIndexParameters<ElementType> GlobalGIParamsType>
		constexpr vec_global_gamma_index_params(const GlobalGIParamsType& other_global_params)
			: vec_global_gamma_index_params(
				math::vectorized::absolute_dose_difference(other_global_params),
				math::vectorized::distance_to_agreement_normalization(other_global_params)
			)
		{ }
		template<GlobalVectorGammaIndexParametersWithSquaredValues<ElementType> GlobalGIParamsType>
		constexpr vec_global_gamma_index_params(const GlobalGIParamsType& other_global_params)
			: vec_global_gamma_index_params(
				math::vectorized::absolute_dose_difference(other_global_params),
				math::vectorized::absolute_dose_difference_squared(other_global_params),
				math::vectorized::distance_to_agreement_normalization(other_global_params),
				math::vectorized::distance_to_agreement_normalization_squared(other_global_params)
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

	template<size_t VectorSize, typename ElementType, GlobalGammaIndexParameters<ElementType> GlobalGIParamsType>
	vec_global_gamma_index_params<ElementType, VectorSize> prepare_vectorized_gi_params(const GlobalGIParamsType& global_gi_params)
	{
		return { global_gi_params };
	}

	template<size_t VectorSize, typename ElementType, GlobalVectorGammaIndexParameters<ElementType, VectorSize> GlobalGIParamsType>
	vec_global_gamma_index_params<ElementType, VectorSize> prepare_vectorized_gi_params(const GlobalGIParamsType& global_gi_params)
	{
		return { global_gi_params };
	}

	template<bool Alignment, typename ElementType>
	auto load(const ElementType* p)
	{
		if constexpr (Alignment)
			return simdpp::load(p);
		else
			return simdpp::load_u(p);
	}

	template<bool Alignment, unsigned N, typename ElementType, typename E>
	void store(ElementType* p, const simdpp::any_vec<N, E>& v)
	{
		if constexpr (Alignment)
			simdpp::store(p, v);
		else
			simdpp::store_u(p, v);
	}

	namespace detail
	{
		template<unsigned VectorSize, typename F>
		inline float find_if(const simdpp::float32<VectorSize>& v, F f, index_sequence<>)
		{
			return std::nanf("");
		}

		template<unsigned VectorSize, typename F, size_t I0, size_t... I>
		inline float find_if(const simdpp::float32<VectorSize>& v, F f, index_sequence<I0, I...>)
		{
			auto value = simdpp::extract<I0>(v);
			if (f(value))
				return value;
			else
				return find_if(v, f, index_sequence<I...>{});
		}

		template<unsigned VectorSize, typename F>
		inline double find_if(const simdpp::float64<VectorSize>& v, F f, index_sequence<>)
		{
			return std::nan("");
		}

		template<unsigned VectorSize, typename F, size_t I0, size_t... I>
		inline double find_if(const simdpp::float64<VectorSize>& v, F f, index_sequence<I0, I...>)
		{
			auto value = simdpp::extract<I0>(v);
			if (f(value))
				return value;
			else
				return find_if(v, f, index_sequence<I...>{});
		}
	}
	
	template<unsigned VectorSize, typename F>
	inline float find_if(const simdpp::float32<VectorSize>& v, F f)
	{
		return detail::find_if(v, f, make_index_sequence<VectorSize>());
	}

	template<unsigned VectorSize, typename F>
	inline double find_if(const simdpp::float64<VectorSize>& v, F f)
	{
		return detail::find_if(v, f, make_index_sequence<VectorSize>());
	}

	template<typename ElementType>
	struct fast_vector_size : std::integral_constant<size_t, 1> {};
	template<>
	struct fast_vector_size<int8_t> : std::integral_constant<size_t, SIMDPP_FAST_INT8_SIZE> {};
	template<>
	struct fast_vector_size<int16_t> : std::integral_constant<size_t, SIMDPP_FAST_INT16_SIZE> {};
	template<>
	struct fast_vector_size<int32_t> : std::integral_constant<size_t, SIMDPP_FAST_INT32_SIZE> {};
	template<>
	struct fast_vector_size<int64_t> : std::integral_constant<size_t, SIMDPP_FAST_INT64_SIZE> {};
	template<>
	struct fast_vector_size<uint8_t> : std::integral_constant<size_t, SIMDPP_FAST_INT8_SIZE> {};
	template<>
	struct fast_vector_size<uint16_t> : std::integral_constant<size_t, SIMDPP_FAST_INT16_SIZE> {};
	template<>
	struct fast_vector_size<uint32_t> : std::integral_constant<size_t, SIMDPP_FAST_INT32_SIZE> {};
	template<>
	struct fast_vector_size<uint64_t> : std::integral_constant<size_t, SIMDPP_FAST_INT64_SIZE> {};
	template<>
	struct fast_vector_size<float> : std::integral_constant<size_t, SIMDPP_FAST_FLOAT32_SIZE> {};
	template<>
	struct fast_vector_size<double> : std::integral_constant<size_t, SIMDPP_FAST_FLOAT64_SIZE> {};

	template<typename ElementType>
	constexpr size_t fast_vector_size_v = fast_vector_size<ElementType>::value;
}