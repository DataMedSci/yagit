#include <boost/test/unit_test.hpp>

#include <math/single/basic/gamma_index_single.hpp>
#include <math/single/vectorized/gamma_index_single.hpp>

#include <random>
#include <chrono>

using namespace yagit::core::math;
using namespace yagit::core::math::single;
using namespace yagit::core::data;
using namespace std;
using namespace std::chrono;
using namespace yagit::core::math::execution;

/*
BOOST_AUTO_TEST_CASE(single_relative_correctness_float)
{
	BOOST_TEST_MESSAGE("------------ single_relative_correctness_float ------------");

	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<float> uniform_dist(0.0, 10.0f);

	constexpr size_t S = 16;
	constexpr float epsilon = 1e-4;

	array<float, S> output_seq;
	array<float, S> output_unseq;

	array<float, S> ref_doses;
	array<float, S> ref_x;
	array<float, S> ref_y;
	array<float, S> ref_z;

	array<float, S> tar_doses;
	array<float, S> tar_x;
	array<float, S> tar_y;
	array<float, S> tar_z;

	auto gen = [&]() {return uniform_dist(e1); };

	std::generate_n(ref_doses.data(), S, gen);
	std::generate_n(ref_x.data(), S, gen);
	std::generate_n(ref_y.data(), S, gen);
	std::generate_n(ref_z.data(), S, gen);

	std::generate_n(tar_doses.data(), S, gen);
	std::generate_n(tar_x.data(), S, gen);
	std::generate_n(tar_y.data(), S, gen);
	std::generate_n(tar_z.data(), S, gen);

	BOOST_TEST_MESSAGE("-------- local --------");
	{
		local_gamma_index_params<float> params{ 0.5f, 12.0f };
		vectorized::vlocal_gamma_index_params<float, S> vparams{ params };
		BOOST_TEST_MESSAGE("---- 1D ----");
		{
			for (size_t i = 0; i < S; i++)
				output_seq[i] = basic::gamma_index(ref_doses[i], tar_doses[i], ref_x[i], tar_x[i], params);

			simdpp::store_u(output_unseq.data(),
				vectorized::gamma_index(
					(simdpp::float32<S>)simdpp::load_u(ref_doses.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_doses.data()),
					(simdpp::float32<S>)simdpp::load_u(ref_x.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_x.data()),
					vparams
				));

			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 2D ----");
		{
			for (size_t i = 0; i < S; i++)
				output_seq[i] = basic::gamma_index(ref_doses[i], tar_doses[i], ref_x[i], ref_y[i], tar_x[i], tar_y[i], params);

			simdpp::store_u(output_unseq.data(),
				vectorized::gamma_index(
					(simdpp::float32<S>)simdpp::load_u(ref_doses.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_doses.data()),
					(simdpp::float32<S>)simdpp::load_u(ref_x.data()),
					(simdpp::float32<S>)simdpp::load_u(ref_y.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_x.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_y.data()),
					vparams
				));

			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 3D ----");
		{
			for (size_t i = 0; i < S; i++)
				output_seq[i] = basic::gamma_index(ref_doses[i], tar_doses[i], ref_x[i], ref_y[i], ref_z[i], tar_x[i], tar_y[i], tar_z[i], params);

			simdpp::store_u(output_unseq.data(),
				vectorized::gamma_index(
					(simdpp::float32<S>)simdpp::load_u(ref_doses.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_doses.data()),
					(simdpp::float32<S>)simdpp::load_u(ref_x.data()),
					(simdpp::float32<S>)simdpp::load_u(ref_y.data()),
					(simdpp::float32<S>)simdpp::load_u(ref_z.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_x.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_y.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_z.data()),
					vparams
				));

			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
		}
	}
	BOOST_TEST_MESSAGE("-------- global --------");
	{
		global_gamma_index_params<float> params{ 1.0f, 2.0f };
		vectorized::vglobal_gamma_index_params<float, S> vparams{ params };
		BOOST_TEST_MESSAGE("---- 1D ----");
		{
			for (size_t i = 0; i < S; i++)
				output_seq[i] = basic::gamma_index(ref_doses[i], tar_doses[i], ref_x[i], tar_x[i], params);

			simdpp::store_u(output_unseq.data(),
				vectorized::gamma_index(
					(simdpp::float32<S>)simdpp::load_u(ref_doses.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_doses.data()),
					(simdpp::float32<S>)simdpp::load_u(ref_x.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_x.data()),
					vparams
				));

			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 2D ----");
		{
			for (size_t i = 0; i < S; i++)
				output_seq[i] = basic::gamma_index(ref_doses[i], tar_doses[i], ref_x[i], ref_y[i], tar_x[i], tar_y[i], params);

			simdpp::store_u(output_unseq.data(),
				vectorized::gamma_index(
					(simdpp::float32<S>)simdpp::load_u(ref_doses.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_doses.data()),
					(simdpp::float32<S>)simdpp::load_u(ref_x.data()),
					(simdpp::float32<S>)simdpp::load_u(ref_y.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_x.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_y.data()),
					vparams
				));

			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 3D ----");
		{
			for (size_t i = 0; i < S; i++)
				output_seq[i] = basic::gamma_index(ref_doses[i], tar_doses[i], ref_x[i], ref_y[i], ref_z[i], tar_x[i], tar_y[i], tar_z[i], params);

			simdpp::store_u(output_unseq.data(),
				vectorized::gamma_index(
					(simdpp::float32<S>)simdpp::load_u(ref_doses.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_doses.data()),
					(simdpp::float32<S>)simdpp::load_u(ref_x.data()),
					(simdpp::float32<S>)simdpp::load_u(ref_y.data()),
					(simdpp::float32<S>)simdpp::load_u(ref_z.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_x.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_y.data()),
					(simdpp::float32<S>)simdpp::load_u(tar_z.data()),
					vparams
				));

			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
		}
	}
}

BOOST_AUTO_TEST_CASE(single_relative_correctness_double)
{
	BOOST_TEST_MESSAGE("------------ single_relative_correctness_double ------------");

	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<double> uniform_dist(0.0, 10.0f);

	constexpr size_t S = 16;
	constexpr double epsilon = 1e-4;

	array<double, S> output_seq;
	array<double, S> output_unseq;

	array<double, S> ref_doses;
	array<double, S> ref_x;
	array<double, S> ref_y;
	array<double, S> ref_z;

	array<double, S> tar_doses;
	array<double, S> tar_x;
	array<double, S> tar_y;
	array<double, S> tar_z;

	auto gen = [&]() {return uniform_dist(e1); };

	std::generate_n(ref_doses.data(), S, gen);
	std::generate_n(ref_x.data(), S, gen);
	std::generate_n(ref_y.data(), S, gen);
	std::generate_n(ref_z.data(), S, gen);

	std::generate_n(tar_doses.data(), S, gen);
	std::generate_n(tar_x.data(), S, gen);
	std::generate_n(tar_y.data(), S, gen);
	std::generate_n(tar_z.data(), S, gen);

	BOOST_TEST_MESSAGE("-------- local --------");
	{
		local_gamma_index_params<double> params{ 0.5f, 12.0f };
		vectorized::vlocal_gamma_index_params<double, S> vparams{ params };
		BOOST_TEST_MESSAGE("---- 1D ----");
		{
			for (size_t i = 0; i < S; i++)
				output_seq[i] = basic::gamma_index(ref_doses[i], tar_doses[i], ref_x[i], tar_x[i], params);

			simdpp::store_u(output_unseq.data(),
				vectorized::gamma_index(
					(simdpp::float64<S>)simdpp::load_u(ref_doses.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_doses.data()),
					(simdpp::float64<S>)simdpp::load_u(ref_x.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_x.data()),
					vparams
				));

			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 2D ----");
		{
			for (size_t i = 0; i < S; i++)
				output_seq[i] = basic::gamma_index(ref_doses[i], tar_doses[i], ref_x[i], ref_y[i], tar_x[i], tar_y[i], params);

			simdpp::store_u(output_unseq.data(),
				vectorized::gamma_index(
					(simdpp::float64<S>)simdpp::load_u(ref_doses.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_doses.data()),
					(simdpp::float64<S>)simdpp::load_u(ref_x.data()),
					(simdpp::float64<S>)simdpp::load_u(ref_y.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_x.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_y.data()),
					vparams
				));

			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 3D ----");
		{
			for (size_t i = 0; i < S; i++)
				output_seq[i] = basic::gamma_index(ref_doses[i], tar_doses[i], ref_x[i], ref_y[i], ref_z[i], tar_x[i], tar_y[i], tar_z[i], params);

			simdpp::store_u(output_unseq.data(),
				vectorized::gamma_index(
					(simdpp::float64<S>)simdpp::load_u(ref_doses.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_doses.data()),
					(simdpp::float64<S>)simdpp::load_u(ref_x.data()),
					(simdpp::float64<S>)simdpp::load_u(ref_y.data()),
					(simdpp::float64<S>)simdpp::load_u(ref_z.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_x.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_y.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_z.data()),
					vparams
				));

			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
		}
	}
	BOOST_TEST_MESSAGE("-------- global --------");
	{
		global_gamma_index_params<double> params{ 0.5f, 12.0f };
		vectorized::vglobal_gamma_index_params<double, S> vparams{ params };
		BOOST_TEST_MESSAGE("---- 1D ----");
		{
			for (size_t i = 0; i < S; i++)
				output_seq[i] = basic::gamma_index(ref_doses[i], tar_doses[i], ref_x[i], tar_x[i], params);

			simdpp::store_u(output_unseq.data(),
				vectorized::gamma_index(
					(simdpp::float64<S>)simdpp::load_u(ref_doses.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_doses.data()),
					(simdpp::float64<S>)simdpp::load_u(ref_x.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_x.data()),
					vparams
				));

			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 2D ----");
		{
			for (size_t i = 0; i < S; i++)
				output_seq[i] = basic::gamma_index(ref_doses[i], tar_doses[i], ref_x[i], ref_y[i], tar_x[i], tar_y[i], params);

			simdpp::store_u(output_unseq.data(),
				vectorized::gamma_index(
					(simdpp::float64<S>)simdpp::load_u(ref_doses.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_doses.data()),
					(simdpp::float64<S>)simdpp::load_u(ref_x.data()),
					(simdpp::float64<S>)simdpp::load_u(ref_y.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_x.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_y.data()),
					vparams
				));

			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 3D ----");
		{
			for (size_t i = 0; i < S; i++)
				output_seq[i] = basic::gamma_index(ref_doses[i], tar_doses[i], ref_x[i], ref_y[i], ref_z[i], tar_x[i], tar_y[i], tar_z[i], params);

			simdpp::store_u(output_unseq.data(),
				vectorized::gamma_index(
					(simdpp::float64<S>)simdpp::load_u(ref_doses.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_doses.data()),
					(simdpp::float64<S>)simdpp::load_u(ref_x.data()),
					(simdpp::float64<S>)simdpp::load_u(ref_y.data()),
					(simdpp::float64<S>)simdpp::load_u(ref_z.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_x.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_y.data()),
					(simdpp::float64<S>)simdpp::load_u(tar_z.data()),
					vparams
				));

			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
		}
	}
}
*/