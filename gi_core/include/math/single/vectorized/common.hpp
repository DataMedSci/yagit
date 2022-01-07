#pragma once

#include <math/common.hpp>

#include <simdpp/simd.h>

namespace yagit::core::math::single::vectorized
{
	template<typename Type, unsigned VectorSize>
	struct vlocal_gamma_index_params {};

	template<unsigned VectorSize>
	struct vlocal_gamma_index_params<float, VectorSize>
	{
		using value_type = float;
		using vector_type = simdpp::float32<VectorSize>;
		static constexpr size_t vector_size = VectorSize;

		vlocal_gamma_index_params(const local_gamma_index_params<value_type>& params)
			: percentage(simdpp::load_splat(&params.percentage))
			, distance_to_agreement_squared(simdpp::load_splat(&params.distance_to_agreement_squared))
		{
		}

		vector_type percentage;
		vector_type distance_to_agreement_squared;
	};

	template<unsigned VectorSize>
	struct vlocal_gamma_index_params<double, VectorSize>
	{
		using value_type = double;
		using vector_type = simdpp::float64<VectorSize>;
		static constexpr size_t vector_size = VectorSize;

		vlocal_gamma_index_params(const local_gamma_index_params<value_type> & params)
			: percentage(simdpp::load_splat(&params.percentage))
			, distance_to_agreement_squared(simdpp::load_splat(&params.distance_to_agreement_squared))
		{
		}

		vector_type percentage;
		vector_type distance_to_agreement_squared;
	};

	template<typename Type, unsigned VectorSize>
	struct vglobal_gamma_index_params {};

	template<unsigned VectorSize>
	struct vglobal_gamma_index_params<float, VectorSize>
	{
		using value_type = float;
		using vector_type = simdpp::float32<VectorSize>;
		static constexpr size_t vector_size = VectorSize;

		vglobal_gamma_index_params(const global_gamma_index_params<value_type>& params)
			: dose_difference_squared(simdpp::load_splat(&params.dose_difference_squared))
			, distance_to_agreement_squared(simdpp::load_splat(&params.distance_to_agreement_squared))
		{
		}

		vector_type dose_difference_squared;
		vector_type distance_to_agreement_squared;
	};

	template<unsigned VectorSize>
	struct vglobal_gamma_index_params<double, VectorSize>
	{
		using value_type = double;
		using vector_type = simdpp::float64<VectorSize>;
		static constexpr size_t vector_size = VectorSize;

		vglobal_gamma_index_params(const global_gamma_index_params<value_type>& params)
			: dose_difference_squared(simdpp::load_splat(&params.dose_difference_squared))
			, distance_to_agreement_squared(simdpp::load_splat(&params.distance_to_agreement_squared))
		{
		}

		vector_type dose_difference_squared;
		vector_type distance_to_agreement_squared;
	};

	template<typename T, unsigned VectorSize>
	struct vgamma_index_params {};

	template<unsigned VectorSize>
	struct vgamma_index_params<float, VectorSize> 
	{
		using value_type = float;
		using vector_type = simdpp::float32<VectorSize>;
		static constexpr size_t vector_size = VectorSize;
		using variant_type = variant<vlocal_gamma_index_params<value_type, VectorSize>, vglobal_gamma_index_params<value_type, VectorSize>>;

		vgamma_index_params(const gamma_index_params<value_type>& params_)
			: params(std::visit([](auto&& param) -> variant_type {return param; }, params_.params))
		{
		}

		variant_type params;
	};

	template<unsigned VectorSize>
	struct vgamma_index_params<double, VectorSize>
	{
		using value_type = double;
		using vector_type = simdpp::float64<VectorSize>;
		static constexpr size_t vector_size = VectorSize;
		using variant_type = variant<vlocal_gamma_index_params<value_type, VectorSize>, vglobal_gamma_index_params<value_type, VectorSize>>;

		vgamma_index_params(const gamma_index_params<value_type>& params_)
			: params(std::visit([](auto&& param) -> variant_type {return param; }, params_.params))
		{
		}

		variant_type params;
	};

	template<typename Type, unsigned VectorSize>
	struct vec {};

	template<unsigned VectorSize>
	struct vec<float, VectorSize> { using type = simdpp::float32<VectorSize>; };

	template<unsigned VectorSize>
	struct vec<double, VectorSize> { using type = simdpp::float64<VectorSize>; };

	template<typename Type, unsigned VectorSize>
	using vec_t = typename vec<Type, VectorSize>::type;

	template<typename ParamsType, unsigned VectorSize>
	struct params_vec {};

	template<typename Type, unsigned VectorSize>
	struct params_vec<local_gamma_index_params<Type>, VectorSize> { using type = vlocal_gamma_index_params<Type, VectorSize>; };

	template<typename Type, unsigned VectorSize>
	struct params_vec<global_gamma_index_params<Type>, VectorSize> { using type = vglobal_gamma_index_params<Type, VectorSize>; };

	template<typename Type, unsigned VectorSize>
	struct params_vec<gamma_index_params<Type>, VectorSize> { using type = vgamma_index_params<Type, VectorSize>; };

	template<typename ParamsType, unsigned VectorSize>
	using params_vec_t = typename params_vec<ParamsType, VectorSize>::type;

	template<bool Alignment, typename Type>
	auto load(const Type* p)
	{
		if constexpr (Alignment)
			return simdpp::load(p);
		else
			return simdpp::load_u(p);
	}

	template<bool Alignment, unsigned N, typename Type, typename E>
	void store(Type* p, const simdpp::any_vec<N, E>& v)
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

	template<typename Type>
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

	template<typename Type>
	constexpr size_t fast_vector_size_v = fast_vector_size<Type>::value;

}