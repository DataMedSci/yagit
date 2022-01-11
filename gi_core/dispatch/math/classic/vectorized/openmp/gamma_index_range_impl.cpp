// This file is a dynamic-dispatch file used by libsimdpp
// http://p12tic.github.io/libsimdpp/v2.2-dev/libsimdpp/w/arch/dispatch.html

#include <math/classic/vectorized/openmp/gamma_index_range_impl.hpp>

#include <simdpp/dispatch/get_arch_raw_cpuid.h>
#define SIMDPP_USER_ARCH_INFO ::simdpp::get_arch_raw_cpuid()

namespace yagit::core::math::classic::vectorized::detail
{
	namespace SIMDPP_ARCH_NAMESPACE
	{
		// -------- float32 --------

		IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(float, 1, UNPACK(input_uniform_image_view<float, 1>), UNPACK(input_uniform_image_view<float, 1>), local_gamma_index_params<float>);
		IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(float, 2, UNPACK(input_uniform_image_view<float, 2>), UNPACK(input_uniform_image_view<float, 2>), local_gamma_index_params<float>);
		IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(float, 3, UNPACK(input_uniform_image_view<float, 3>), UNPACK(input_uniform_image_view<float, 3>), local_gamma_index_params<float>);
		IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(float, 1, UNPACK(input_uniform_image_view<float, 1>), UNPACK(input_uniform_image_view<float, 1>), global_gamma_index_params<float>);
		IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(float, 2, UNPACK(input_uniform_image_view<float, 2>), UNPACK(input_uniform_image_view<float, 2>), global_gamma_index_params<float>);
		IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(float, 3, UNPACK(input_uniform_image_view<float, 3>), UNPACK(input_uniform_image_view<float, 3>), global_gamma_index_params<float>);

		// -------- float64 --------

		IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(double, 1, UNPACK(input_uniform_image_view<double, 1>), UNPACK(input_uniform_image_view<double, 1>), local_gamma_index_params<double>);
		IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(double, 2, UNPACK(input_uniform_image_view<double, 2>), UNPACK(input_uniform_image_view<double, 2>), local_gamma_index_params<double>);
		IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(double, 3, UNPACK(input_uniform_image_view<double, 3>), UNPACK(input_uniform_image_view<double, 3>), local_gamma_index_params<double>);
		IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(double, 1, UNPACK(input_uniform_image_view<double, 1>), UNPACK(input_uniform_image_view<double, 1>), global_gamma_index_params<double>);
		IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(double, 2, UNPACK(input_uniform_image_view<double, 2>), UNPACK(input_uniform_image_view<double, 2>), global_gamma_index_params<double>);
		IMPLEMENT_CLASSIC_PAR_UNSEQ_GI(double, 3, UNPACK(input_uniform_image_view<double, 3>), UNPACK(input_uniform_image_view<double, 3>), global_gamma_index_params<double>);
	}

	// -------- float32 --------

	MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(float, 1, UNPACK(input_uniform_image_view<float, 1>), UNPACK(input_uniform_image_view<float, 1>), local_gamma_index_params<float>);
	MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(float, 2, UNPACK(input_uniform_image_view<float, 2>), UNPACK(input_uniform_image_view<float, 2>), local_gamma_index_params<float>);
	MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(float, 3, UNPACK(input_uniform_image_view<float, 3>), UNPACK(input_uniform_image_view<float, 3>), local_gamma_index_params<float>);
	MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(float, 1, UNPACK(input_uniform_image_view<float, 1>), UNPACK(input_uniform_image_view<float, 1>), global_gamma_index_params<float>);
	MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(float, 2, UNPACK(input_uniform_image_view<float, 2>), UNPACK(input_uniform_image_view<float, 2>), global_gamma_index_params<float>);
	MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(float, 3, UNPACK(input_uniform_image_view<float, 3>), UNPACK(input_uniform_image_view<float, 3>), global_gamma_index_params<float>);

	// -------- float64 --------

	MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(double, 1, UNPACK(input_uniform_image_view<double, 1>), UNPACK(input_uniform_image_view<double, 1>), local_gamma_index_params<double>);
	MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(double, 2, UNPACK(input_uniform_image_view<double, 2>), UNPACK(input_uniform_image_view<double, 2>), local_gamma_index_params<double>);
	MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(double, 3, UNPACK(input_uniform_image_view<double, 3>), UNPACK(input_uniform_image_view<double, 3>), local_gamma_index_params<double>);
	MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(double, 1, UNPACK(input_uniform_image_view<double, 1>), UNPACK(input_uniform_image_view<double, 1>), global_gamma_index_params<double>);
	MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(double, 2, UNPACK(input_uniform_image_view<double, 2>), UNPACK(input_uniform_image_view<double, 2>), global_gamma_index_params<double>);
	MAKE_CLASSIC_PAR_UNSEQ_DISPATCHER(double, 3, UNPACK(input_uniform_image_view<double, 3>), UNPACK(input_uniform_image_view<double, 3>), global_gamma_index_params<double>);
}