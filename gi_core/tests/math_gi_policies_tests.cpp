#include <math/gamma_index.hpp>
#include <image.h>
#include <solver.h>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE  math_tests
#include <boost/test/unit_test.hpp>

#include <random>
#include <chrono>

using namespace yagit::core::math;
using namespace yagit::core::data;
using namespace std;
using namespace std::chrono;

BOOST_AUTO_TEST_CASE(policies_relative_correctness_float)
{
	BOOST_TEST_MESSAGE("------------ policies_relative_correctness_float ------------");

	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<float> uniform_dist(0.0, 10.0f);

	constexpr size_t S = 1000;
	constexpr float epsilon = 1e-4;

	vector<float> output_seq(S, 0.0f);
	vector<float> output_unseq(S, 0.0f);
	vector<float> output_par(S, 0.0f);
	vector<float> output_par_unseq(S, 0.0f);

	vector<float> ref_doses(S, 0.0f);
	vector<float> ref_x(S, 0.0f);
	vector<float> ref_y(S, 0.0f);
	vector<float> ref_z(S, 0.0f);

	vector<float> tar_doses(S, 0.0f);
	vector<float> tar_x(S, 0.0f);
	vector<float> tar_y(S, 0.0f);
	vector<float> tar_z(S, 0.0f);

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
		BOOST_TEST_MESSAGE("---- 1D ----");
		// sequenced
		{
			auto start = high_resolution_clock::now();
			sequenced_gamma_index_implementer<float, 1>::initialize_pass(output_seq.data(), output_seq.data() + S);
			sequenced_gamma_index_implementer<float, 1>::minimize_pass(
				output_seq.data(), output_seq.data() + S,
				ref_doses.data(),
				array<const float*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 1>{tar_x.data()},
				params
			);
			sequenced_gamma_index_implementer<float, 1>::finalize_pass(output_seq.data(), output_seq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("sequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// unsequenced
		{
			auto start = high_resolution_clock::now();
			unsequenced_gamma_index_implementer<float, 1>::initialize_pass(output_unseq.data(), output_unseq.data() + S);
			unsequenced_gamma_index_implementer<float, 1>::minimize_pass(
				output_unseq.data(), output_unseq.data() + S,
				ref_doses.data(),
				array<const float*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 1>{tar_x.data()},
				params
			);
			unsequenced_gamma_index_implementer<float, 1>::finalize_pass(output_unseq.data(), output_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel
		{
			auto start = high_resolution_clock::now();
			parallel_gamma_index_implementer<float, 1>::initialize_pass(output_par.data(), output_par.data() + S);
			parallel_gamma_index_implementer<float, 1>::minimize_pass(
				output_par.data(), output_par.data() + S,
				ref_doses.data(),
				array<const float*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 1>{tar_x.data()},
				params
			);
			parallel_gamma_index_implementer<float, 1>::finalize_pass(output_par.data(), output_par.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel unsequenced
		{
			auto start = high_resolution_clock::now();
			parallel_unsequenced_gamma_index_implementer<float, 1>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			parallel_unsequenced_gamma_index_implementer<float, 1>::minimize_pass(
				output_par_unseq.data(), output_par_unseq.data() + S,
				ref_doses.data(),
				array<const float*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 1>{tar_x.data()},
				params
			);
			parallel_unsequenced_gamma_index_implementer<float, 1>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// compare
		{
			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);
			auto par_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par.data(), output_par.data() + S, comp);
			auto par_unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par_unseq.data(), output_par_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
			BOOST_CHECK(par_equal_to_seq);
			BOOST_CHECK(par_unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 2D ----");
		// sequenced
		{
			auto start = high_resolution_clock::now();
			sequenced_gamma_index_implementer<float, 2>::initialize_pass(output_seq.data(), output_seq.data() + S);
			sequenced_gamma_index_implementer<float, 2>::minimize_pass(
				output_seq.data(), output_seq.data() + S,
				ref_doses.data(),
				array<const float*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			sequenced_gamma_index_implementer<float, 2>::finalize_pass(output_seq.data(), output_seq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("sequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// unsequenced
		{
			auto start = high_resolution_clock::now();
			unsequenced_gamma_index_implementer<float, 2>::initialize_pass(output_unseq.data(), output_unseq.data() + S);
			unsequenced_gamma_index_implementer<float, 2>::minimize_pass(
				output_unseq.data(), output_unseq.data() + S,
				ref_doses.data(),
				array<const float*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			unsequenced_gamma_index_implementer<float, 2>::finalize_pass(output_unseq.data(), output_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel
		{
			auto start = high_resolution_clock::now();
			parallel_gamma_index_implementer<float, 2>::initialize_pass(output_par.data(), output_par.data() + S);
			parallel_gamma_index_implementer<float, 2>::minimize_pass(
				output_par.data(), output_par.data() + S,
				ref_doses.data(),
				array<const float*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			parallel_gamma_index_implementer<float, 2>::finalize_pass(output_par.data(), output_par.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel unsequenced
		{
			auto start = high_resolution_clock::now();
			parallel_unsequenced_gamma_index_implementer<float, 2>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			parallel_unsequenced_gamma_index_implementer<float, 2>::minimize_pass(
				output_par_unseq.data(), output_par_unseq.data() + S,
				ref_doses.data(),
				array<const float*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			parallel_unsequenced_gamma_index_implementer<float, 2>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// compare
		{
			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);
			auto par_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par.data(), output_par.data() + S, comp);
			auto par_unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par_unseq.data(), output_par_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
			BOOST_CHECK(par_equal_to_seq);
			BOOST_CHECK(par_unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 3D ----");
		// sequenced
		{
			auto start = high_resolution_clock::now();
			sequenced_gamma_index_implementer<float, 3>::initialize_pass(output_seq.data(), output_seq.data() + S);
			sequenced_gamma_index_implementer<float, 3>::minimize_pass(
				output_seq.data(), output_seq.data() + S,
				ref_doses.data(),
				array<const float*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			sequenced_gamma_index_implementer<float, 3>::finalize_pass(output_seq.data(), output_seq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("sequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// unsequenced
		{
			auto start = high_resolution_clock::now();
			unsequenced_gamma_index_implementer<float, 3>::initialize_pass(output_unseq.data(), output_unseq.data() + S);
			unsequenced_gamma_index_implementer<float, 3>::minimize_pass(
				output_unseq.data(), output_unseq.data() + S,
				ref_doses.data(),
				array<const float*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			unsequenced_gamma_index_implementer<float, 3>::finalize_pass(output_unseq.data(), output_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel
		{
			auto start = high_resolution_clock::now();
			parallel_gamma_index_implementer<float, 3>::initialize_pass(output_par.data(), output_par.data() + S);
			parallel_gamma_index_implementer<float, 3>::minimize_pass(
				output_par.data(), output_par.data() + S,
				ref_doses.data(),
				array<const float*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			parallel_gamma_index_implementer<float, 3>::finalize_pass(output_par.data(), output_par.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel unsequenced
		{
			auto start = high_resolution_clock::now();
			parallel_unsequenced_gamma_index_implementer<float, 3>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			parallel_unsequenced_gamma_index_implementer<float, 3>::minimize_pass(
				output_par_unseq.data(), output_par_unseq.data() + S,
				ref_doses.data(),
				array<const float*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			parallel_unsequenced_gamma_index_implementer<float, 3>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// compare
		{
			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);
			auto par_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par.data(), output_par.data() + S, comp);
			auto par_unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par_unseq.data(), output_par_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
			BOOST_CHECK(par_equal_to_seq);
			BOOST_CHECK(par_unseq_equal_to_seq);
		}
	}

	BOOST_TEST_MESSAGE("-------- global --------");
	{
		global_gamma_index_params<float> params{ 0.5f, 12.0f };
		BOOST_TEST_MESSAGE("---- 1D ----");
		// sequenced
		{
			auto start = high_resolution_clock::now();
			sequenced_gamma_index_implementer<float, 1>::initialize_pass(output_seq.data(), output_seq.data() + S);
			sequenced_gamma_index_implementer<float, 1>::minimize_pass(
				output_seq.data(), output_seq.data() + S,
				ref_doses.data(),
				array<const float*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 1>{tar_x.data()},
				params
			);
			sequenced_gamma_index_implementer<float, 1>::finalize_pass(output_seq.data(), output_seq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("sequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// unsequenced
		{
			auto start = high_resolution_clock::now();
			unsequenced_gamma_index_implementer<float, 1>::initialize_pass(output_unseq.data(), output_unseq.data() + S);
			unsequenced_gamma_index_implementer<float, 1>::minimize_pass(
				output_unseq.data(), output_unseq.data() + S,
				ref_doses.data(),
				array<const float*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 1>{tar_x.data()},
				params
			);
			unsequenced_gamma_index_implementer<float, 1>::finalize_pass(output_unseq.data(), output_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel
		{
			auto start = high_resolution_clock::now();
			parallel_gamma_index_implementer<float, 1>::initialize_pass(output_par.data(), output_par.data() + S);
			parallel_gamma_index_implementer<float, 1>::minimize_pass(
				output_par.data(), output_par.data() + S,
				ref_doses.data(),
				array<const float*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 1>{tar_x.data()},
				params
			);
			parallel_gamma_index_implementer<float, 1>::finalize_pass(output_par.data(), output_par.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel unsequenced
		{
			auto start = high_resolution_clock::now();
			parallel_unsequenced_gamma_index_implementer<float, 1>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			parallel_unsequenced_gamma_index_implementer<float, 1>::minimize_pass(
				output_par_unseq.data(), output_par_unseq.data() + S,
				ref_doses.data(),
				array<const float*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 1>{tar_x.data()},
				params
			);
			parallel_unsequenced_gamma_index_implementer<float, 1>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// compare
		{
			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);
			auto par_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par.data(), output_par.data() + S, comp);
			auto par_unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par_unseq.data(), output_par_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
			BOOST_CHECK(par_equal_to_seq);
			BOOST_CHECK(par_unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 2D ----");
		// sequenced
		{
			auto start = high_resolution_clock::now();
			sequenced_gamma_index_implementer<float, 2>::initialize_pass(output_seq.data(), output_seq.data() + S);
			sequenced_gamma_index_implementer<float, 2>::minimize_pass(
				output_seq.data(), output_seq.data() + S,
				ref_doses.data(),
				array<const float*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			sequenced_gamma_index_implementer<float, 2>::finalize_pass(output_seq.data(), output_seq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("sequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// unsequenced
		{
			auto start = high_resolution_clock::now();
			unsequenced_gamma_index_implementer<float, 2>::initialize_pass(output_unseq.data(), output_unseq.data() + S);
			unsequenced_gamma_index_implementer<float, 2>::minimize_pass(
				output_unseq.data(), output_unseq.data() + S,
				ref_doses.data(),
				array<const float*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			unsequenced_gamma_index_implementer<float, 2>::finalize_pass(output_unseq.data(), output_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel
		{
			auto start = high_resolution_clock::now();
			parallel_gamma_index_implementer<float, 2>::initialize_pass(output_par.data(), output_par.data() + S);
			parallel_gamma_index_implementer<float, 2>::minimize_pass(
				output_par.data(), output_par.data() + S,
				ref_doses.data(),
				array<const float*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			parallel_gamma_index_implementer<float, 2>::finalize_pass(output_par.data(), output_par.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel unsequenced
		{
			auto start = high_resolution_clock::now();
			parallel_unsequenced_gamma_index_implementer<float, 2>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			parallel_unsequenced_gamma_index_implementer<float, 2>::minimize_pass(
				output_par_unseq.data(), output_par_unseq.data() + S,
				ref_doses.data(),
				array<const float*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			parallel_unsequenced_gamma_index_implementer<float, 2>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// compare
		{
			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);
			auto par_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par.data(), output_par.data() + S, comp);
			auto par_unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par_unseq.data(), output_par_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
			BOOST_CHECK(par_equal_to_seq);
			BOOST_CHECK(par_unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 3D ----");
		// sequenced
		{
			auto start = high_resolution_clock::now();
			sequenced_gamma_index_implementer<float, 3>::initialize_pass(output_seq.data(), output_seq.data() + S);
			sequenced_gamma_index_implementer<float, 3>::minimize_pass(
				output_seq.data(), output_seq.data() + S,
				ref_doses.data(),
				array<const float*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			sequenced_gamma_index_implementer<float, 3>::finalize_pass(output_seq.data(), output_seq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("sequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// unsequenced
		{
			auto start = high_resolution_clock::now();
			unsequenced_gamma_index_implementer<float, 3>::initialize_pass(output_unseq.data(), output_unseq.data() + S);
			unsequenced_gamma_index_implementer<float, 3>::minimize_pass(
				output_unseq.data(), output_unseq.data() + S,
				ref_doses.data(),
				array<const float*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			unsequenced_gamma_index_implementer<float, 3>::finalize_pass(output_unseq.data(), output_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel
		{
			auto start = high_resolution_clock::now();
			parallel_gamma_index_implementer<float, 3>::initialize_pass(output_par.data(), output_par.data() + S);
			parallel_gamma_index_implementer<float, 3>::minimize_pass(
				output_par.data(), output_par.data() + S,
				ref_doses.data(),
				array<const float*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			parallel_gamma_index_implementer<float, 3>::finalize_pass(output_par.data(), output_par.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel unsequenced
		{
			auto start = high_resolution_clock::now();
			parallel_unsequenced_gamma_index_implementer<float, 3>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			parallel_unsequenced_gamma_index_implementer<float, 3>::minimize_pass(
				output_par_unseq.data(), output_par_unseq.data() + S,
				ref_doses.data(),
				array<const float*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const float*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			parallel_unsequenced_gamma_index_implementer<float, 3>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// compare
		{
			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);
			auto par_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par.data(), output_par.data() + S, comp);
			auto par_unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par_unseq.data(), output_par_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
			BOOST_CHECK(par_equal_to_seq);
			BOOST_CHECK(par_unseq_equal_to_seq);
		}
	}
}

BOOST_AUTO_TEST_CASE(policies_relative_correctness_double)
{
	BOOST_TEST_MESSAGE("------------ policies_relative_correctness_double ------------");

	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<double> uniform_dist(0.0, 10.0);

	constexpr size_t S = 1000;
	constexpr double epsilon = 1e-9;

	vector<double> output_seq(S, 0.0);
	vector<double> output_unseq(S, 0.0);
	vector<double> output_par(S, 0.0);
	vector<double> output_par_unseq(S, 0.0);

	vector<double> ref_doses(S, 0.0);
	vector<double> ref_x(S, 0.0);
	vector<double> ref_y(S, 0.0);
	vector<double> ref_z(S, 0.0);

	vector<double> tar_doses(S, 0.0);
	vector<double> tar_x(S, 0.0);
	vector<double> tar_y(S, 0.0);
	vector<double> tar_z(S, 0.0);

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
		BOOST_TEST_MESSAGE("---- 1D ----");
		// sequenced
		{
			auto start = high_resolution_clock::now();
			sequenced_gamma_index_implementer<double, 1>::initialize_pass(output_seq.data(), output_seq.data() + S);
			sequenced_gamma_index_implementer<double, 1>::minimize_pass(
				output_seq.data(), output_seq.data() + S,
				ref_doses.data(),
				array<const double*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 1>{tar_x.data()},
				params
			);
			sequenced_gamma_index_implementer<double, 1>::finalize_pass(output_seq.data(), output_seq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("sequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// unsequenced
		{
			auto start = high_resolution_clock::now();
			unsequenced_gamma_index_implementer<double, 1>::initialize_pass(output_unseq.data(), output_unseq.data() + S);
			unsequenced_gamma_index_implementer<double, 1>::minimize_pass(
				output_unseq.data(), output_unseq.data() + S,
				ref_doses.data(),
				array<const double*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 1>{tar_x.data()},
				params
			);
			unsequenced_gamma_index_implementer<double, 1>::finalize_pass(output_unseq.data(), output_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel
		{
			auto start = high_resolution_clock::now();
			parallel_gamma_index_implementer<double, 1>::initialize_pass(output_par.data(), output_par.data() + S);
			parallel_gamma_index_implementer<double, 1>::minimize_pass(
				output_par.data(), output_par.data() + S,
				ref_doses.data(),
				array<const double*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 1>{tar_x.data()},
				params
			);
			parallel_gamma_index_implementer<double, 1>::finalize_pass(output_par.data(), output_par.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel unsequenced
		{
			auto start = high_resolution_clock::now();
			parallel_unsequenced_gamma_index_implementer<double, 1>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			parallel_unsequenced_gamma_index_implementer<double, 1>::minimize_pass(
				output_par_unseq.data(), output_par_unseq.data() + S,
				ref_doses.data(),
				array<const double*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 1>{tar_x.data()},
				params
			);
			parallel_unsequenced_gamma_index_implementer<double, 1>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// compare
		{
			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);
			auto par_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par.data(), output_par.data() + S, comp);
			auto par_unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par_unseq.data(), output_par_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
			BOOST_CHECK(par_equal_to_seq);
			BOOST_CHECK(par_unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 2D ----");
		// sequenced
		{
			auto start = high_resolution_clock::now();
			sequenced_gamma_index_implementer<double, 2>::initialize_pass(output_seq.data(), output_seq.data() + S);
			sequenced_gamma_index_implementer<double, 2>::minimize_pass(
				output_seq.data(), output_seq.data() + S,
				ref_doses.data(),
				array<const double*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			sequenced_gamma_index_implementer<double, 2>::finalize_pass(output_seq.data(), output_seq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("sequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// unsequenced
		{
			auto start = high_resolution_clock::now();
			unsequenced_gamma_index_implementer<double, 2>::initialize_pass(output_unseq.data(), output_unseq.data() + S);
			unsequenced_gamma_index_implementer<double, 2>::minimize_pass(
				output_unseq.data(), output_unseq.data() + S,
				ref_doses.data(),
				array<const double*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			unsequenced_gamma_index_implementer<double, 2>::finalize_pass(output_unseq.data(), output_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel
		{
			auto start = high_resolution_clock::now();
			parallel_gamma_index_implementer<double, 2>::initialize_pass(output_par.data(), output_par.data() + S);
			parallel_gamma_index_implementer<double, 2>::minimize_pass(
				output_par.data(), output_par.data() + S,
				ref_doses.data(),
				array<const double*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			parallel_gamma_index_implementer<double, 2>::finalize_pass(output_par.data(), output_par.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel unsequenced
		{
			auto start = high_resolution_clock::now();
			parallel_unsequenced_gamma_index_implementer<double, 2>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			parallel_unsequenced_gamma_index_implementer<double, 2>::minimize_pass(
				output_par_unseq.data(), output_par_unseq.data() + S,
				ref_doses.data(),
				array<const double*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			parallel_unsequenced_gamma_index_implementer<double, 2>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// compare
		{
			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);
			auto par_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par.data(), output_par.data() + S, comp);
			auto par_unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par_unseq.data(), output_par_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
			BOOST_CHECK(par_equal_to_seq);
			BOOST_CHECK(par_unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 3D ----");
		// sequenced
		{
			auto start = high_resolution_clock::now();
			sequenced_gamma_index_implementer<double, 3>::initialize_pass(output_seq.data(), output_seq.data() + S);
			sequenced_gamma_index_implementer<double, 3>::minimize_pass(
				output_seq.data(), output_seq.data() + S,
				ref_doses.data(),
				array<const double*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			sequenced_gamma_index_implementer<double, 3>::finalize_pass(output_seq.data(), output_seq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("sequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// unsequenced
		{
			auto start = high_resolution_clock::now();
			unsequenced_gamma_index_implementer<double, 3>::initialize_pass(output_unseq.data(), output_unseq.data() + S);
			unsequenced_gamma_index_implementer<double, 3>::minimize_pass(
				output_unseq.data(), output_unseq.data() + S,
				ref_doses.data(),
				array<const double*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			unsequenced_gamma_index_implementer<double, 3>::finalize_pass(output_unseq.data(), output_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel
		{
			auto start = high_resolution_clock::now();
			parallel_gamma_index_implementer<double, 3>::initialize_pass(output_par.data(), output_par.data() + S);
			parallel_gamma_index_implementer<double, 3>::minimize_pass(
				output_par.data(), output_par.data() + S,
				ref_doses.data(),
				array<const double*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			parallel_gamma_index_implementer<double, 3>::finalize_pass(output_par.data(), output_par.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel unsequenced
		{
			auto start = high_resolution_clock::now();
			parallel_unsequenced_gamma_index_implementer<double, 3>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			parallel_unsequenced_gamma_index_implementer<double, 3>::minimize_pass(
				output_par_unseq.data(), output_par_unseq.data() + S,
				ref_doses.data(),
				array<const double*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			parallel_unsequenced_gamma_index_implementer<double, 3>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// compare
		{
			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);
			auto par_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par.data(), output_par.data() + S, comp);
			auto par_unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par_unseq.data(), output_par_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
			BOOST_CHECK(par_equal_to_seq);
			BOOST_CHECK(par_unseq_equal_to_seq);
		}
	}

	BOOST_TEST_MESSAGE("-------- global --------");
	{
		global_gamma_index_params<double> params{ 0.5f, 12.0f };
		BOOST_TEST_MESSAGE("---- 1D ----");
		// sequenced
		{
			auto start = high_resolution_clock::now();
			sequenced_gamma_index_implementer<double, 1>::initialize_pass(output_seq.data(), output_seq.data() + S);
			sequenced_gamma_index_implementer<double, 1>::minimize_pass(
				output_seq.data(), output_seq.data() + S,
				ref_doses.data(),
				array<const double*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 1>{tar_x.data()},
				params
			);
			sequenced_gamma_index_implementer<double, 1>::finalize_pass(output_seq.data(), output_seq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("sequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// unsequenced
		{
			auto start = high_resolution_clock::now();
			unsequenced_gamma_index_implementer<double, 1>::initialize_pass(output_unseq.data(), output_unseq.data() + S);
			unsequenced_gamma_index_implementer<double, 1>::minimize_pass(
				output_unseq.data(), output_unseq.data() + S,
				ref_doses.data(),
				array<const double*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 1>{tar_x.data()},
				params
			);
			unsequenced_gamma_index_implementer<double, 1>::finalize_pass(output_unseq.data(), output_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel
		{
			auto start = high_resolution_clock::now();
			parallel_gamma_index_implementer<double, 1>::initialize_pass(output_par.data(), output_par.data() + S);
			parallel_gamma_index_implementer<double, 1>::minimize_pass(
				output_par.data(), output_par.data() + S,
				ref_doses.data(),
				array<const double*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 1>{tar_x.data()},
				params
			);
			parallel_gamma_index_implementer<double, 1>::finalize_pass(output_par.data(), output_par.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel unsequenced
		{
			auto start = high_resolution_clock::now();
			parallel_unsequenced_gamma_index_implementer<double, 1>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			parallel_unsequenced_gamma_index_implementer<double, 1>::minimize_pass(
				output_par_unseq.data(), output_par_unseq.data() + S,
				ref_doses.data(),
				array<const double*, 1>{ref_x.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 1>{tar_x.data()},
				params
			);
			parallel_unsequenced_gamma_index_implementer<double, 1>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// compare
		{
			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);
			auto par_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par.data(), output_par.data() + S, comp);
			auto par_unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par_unseq.data(), output_par_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
			BOOST_CHECK(par_equal_to_seq);
			BOOST_CHECK(par_unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 2D ----");
		// sequenced
		{
			auto start = high_resolution_clock::now();
			sequenced_gamma_index_implementer<double, 2>::initialize_pass(output_seq.data(), output_seq.data() + S);
			sequenced_gamma_index_implementer<double, 2>::minimize_pass(
				output_seq.data(), output_seq.data() + S,
				ref_doses.data(),
				array<const double*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			sequenced_gamma_index_implementer<double, 2>::finalize_pass(output_seq.data(), output_seq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("sequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// unsequenced
		{
			auto start = high_resolution_clock::now();
			unsequenced_gamma_index_implementer<double, 2>::initialize_pass(output_unseq.data(), output_unseq.data() + S);
			unsequenced_gamma_index_implementer<double, 2>::minimize_pass(
				output_unseq.data(), output_unseq.data() + S,
				ref_doses.data(),
				array<const double*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			unsequenced_gamma_index_implementer<double, 2>::finalize_pass(output_unseq.data(), output_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel
		{
			auto start = high_resolution_clock::now();
			parallel_gamma_index_implementer<double, 2>::initialize_pass(output_par.data(), output_par.data() + S);
			parallel_gamma_index_implementer<double, 2>::minimize_pass(
				output_par.data(), output_par.data() + S,
				ref_doses.data(),
				array<const double*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			parallel_gamma_index_implementer<double, 2>::finalize_pass(output_par.data(), output_par.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel unsequenced
		{
			auto start = high_resolution_clock::now();
			parallel_unsequenced_gamma_index_implementer<double, 2>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			parallel_unsequenced_gamma_index_implementer<double, 2>::minimize_pass(
				output_par_unseq.data(), output_par_unseq.data() + S,
				ref_doses.data(),
				array<const double*, 2>{ref_x.data(), ref_y.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 2>{tar_x.data(), tar_y.data()},
				params
			);
			parallel_unsequenced_gamma_index_implementer<double, 2>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// compare
		{
			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);
			auto par_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par.data(), output_par.data() + S, comp);
			auto par_unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par_unseq.data(), output_par_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
			BOOST_CHECK(par_equal_to_seq);
			BOOST_CHECK(par_unseq_equal_to_seq);
		}
		BOOST_TEST_MESSAGE("---- 3D ----");
		// sequenced
		{
			auto start = high_resolution_clock::now();
			sequenced_gamma_index_implementer<double, 3>::initialize_pass(output_seq.data(), output_seq.data() + S);
			sequenced_gamma_index_implementer<double, 3>::minimize_pass(
				output_seq.data(), output_seq.data() + S,
				ref_doses.data(),
				array<const double*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			sequenced_gamma_index_implementer<double, 3>::finalize_pass(output_seq.data(), output_seq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("sequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// unsequenced
		{
			auto start = high_resolution_clock::now();
			unsequenced_gamma_index_implementer<double, 3>::initialize_pass(output_unseq.data(), output_unseq.data() + S);
			unsequenced_gamma_index_implementer<double, 3>::minimize_pass(
				output_unseq.data(), output_unseq.data() + S,
				ref_doses.data(),
				array<const double*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			unsequenced_gamma_index_implementer<double, 3>::finalize_pass(output_unseq.data(), output_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel
		{
			auto start = high_resolution_clock::now();
			parallel_gamma_index_implementer<double, 3>::initialize_pass(output_par.data(), output_par.data() + S);
			parallel_gamma_index_implementer<double, 3>::minimize_pass(
				output_par.data(), output_par.data() + S,
				ref_doses.data(),
				array<const double*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			parallel_gamma_index_implementer<double, 3>::finalize_pass(output_par.data(), output_par.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// parallel unsequenced
		{
			auto start = high_resolution_clock::now();
			parallel_unsequenced_gamma_index_implementer<double, 3>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			parallel_unsequenced_gamma_index_implementer<double, 3>::minimize_pass(
				output_par_unseq.data(), output_par_unseq.data() + S,
				ref_doses.data(),
				array<const double*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
				tar_doses.data(), tar_doses.data() + S,
				array<const double*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
				params
			);
			parallel_unsequenced_gamma_index_implementer<double, 3>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
			auto end = high_resolution_clock::now();
			BOOST_TEST_MESSAGE("parallel unsequenced took: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
		}
		// compare
		{
			auto comp = [](auto a, auto b) {return std::abs(a - b) < epsilon; };
			auto unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_unseq.data(), output_unseq.data() + S, comp);
			auto par_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par.data(), output_par.data() + S, comp);
			auto par_unseq_equal_to_seq = equal(execution::par_unseq, output_seq.data(), output_seq.data() + S, output_par_unseq.data(), output_par_unseq.data() + S, comp);

			BOOST_CHECK(unseq_equal_to_seq);
			BOOST_CHECK(par_equal_to_seq);
			BOOST_CHECK(par_unseq_equal_to_seq);
		}
	}
}

BOOST_AUTO_TEST_CASE(performance_check_float)
{
	BOOST_TEST_MESSAGE("------------ performance_check_float parallel_unsequenced only ------------");
	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<float> uniform_dist(0.0, 10.0f);

	constexpr size_t S = 1000000;
	constexpr float epsilon = 1e-4;

	vector<float> output_par_unseq(S, 0.0f);

	vector<float> ref_doses(S, 0.0f);
	vector<float> ref_x(S, 0.0f);
	vector<float> ref_y(S, 0.0f);
	vector<float> ref_z(S, 0.0f);

	vector<float> tar_doses(S, 0.0f);
	vector<float> tar_x(S, 0.0f);
	vector<float> tar_y(S, 0.0f);
	vector<float> tar_z(S, 0.0f);

	auto gen = [&]() {return uniform_dist(e1); };

	std::generate_n(ref_doses.data(), S, gen);
	std::generate_n(ref_x.data(), S, gen);
	std::generate_n(ref_y.data(), S, gen);
	std::generate_n(ref_z.data(), S, gen);

	std::generate_n(tar_doses.data(), S, gen);
	std::generate_n(tar_x.data(), S, gen);
	std::generate_n(tar_y.data(), S, gen);
	std::generate_n(tar_z.data(), S, gen);

	local_gamma_index_params<float> params{ 0.5f, 12.0f };

	auto start = high_resolution_clock::now();
	parallel_unsequenced_gamma_index_implementer<float, 3>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
	parallel_unsequenced_gamma_index_implementer<float, 3>::minimize_pass(
		output_par_unseq.data(), output_par_unseq.data() + S,
		ref_doses.data(),
		array<const float*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
		tar_doses.data(), tar_doses.data() + S,
		array<const float*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
		params
	);
	parallel_unsequenced_gamma_index_implementer<float, 3>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
	auto end = high_resolution_clock::now();
	BOOST_TEST_MESSAGE("parallel unsequenced took (1000x1000 random image): " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
}

BOOST_AUTO_TEST_CASE(performance_check_double)
{
	BOOST_TEST_MESSAGE("------------ performance_check_double parallel_unsequenced only ------------");
	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<double> uniform_dist(0.0, 10.0f);

	constexpr size_t S = 10000000;
	constexpr double epsilon = 1e-9;

	vector<double> output_par_unseq(S, 0.0);

	vector<double> ref_doses(S, 0.0);
	vector<double> ref_x(S, 0.0);
	vector<double> ref_y(S, 0.0);
	vector<double> ref_z(S, 0.0);

	vector<double> tar_doses(S, 0.0);
	vector<double> tar_x(S, 0.0);
	vector<double> tar_y(S, 0.0);
	vector<double> tar_z(S, 0.0);

	auto gen = [&]() {return uniform_dist(e1); };

	std::generate_n(ref_doses.data(), S, gen);
	std::generate_n(ref_x.data(), S, gen);
	std::generate_n(ref_y.data(), S, gen);
	std::generate_n(ref_z.data(), S, gen);

	std::generate_n(tar_doses.data(), S, gen);
	std::generate_n(tar_x.data(), S, gen);
	std::generate_n(tar_y.data(), S, gen);
	std::generate_n(tar_z.data(), S, gen);

	local_gamma_index_params<double> params{ 0.5f, 12.0f };

	auto start = high_resolution_clock::now();
	parallel_unsequenced_gamma_index_implementer<double, 3>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
	parallel_unsequenced_gamma_index_implementer<double, 3>::minimize_pass(
		output_par_unseq.data(), output_par_unseq.data() + S,
		ref_doses.data(),
		array<const double*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
		tar_doses.data(), tar_doses.data() + S,
		array<const double*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
		params
	);
	parallel_unsequenced_gamma_index_implementer<double, 3>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
	auto end = high_resolution_clock::now();
	BOOST_TEST_MESSAGE("parallel unsequenced took (1000x1000 random image): " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
}

BOOST_AUTO_TEST_CASE(performance_vs_original_spiral_rectangle)
{
	BOOST_TEST_MESSAGE("------------ performance_vs_original_spiral_rectangle parallel_unsequenced vs spiral_rectangle ------------");
	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<double> uniform_dist(0.0, 10.0f);
	auto gen = [&]() {return uniform_dist(e1); };

	constexpr size_t SX = 100;
	constexpr size_t SY = 10;
	constexpr size_t SZ = 10;
	constexpr size_t S = SX * SY * SZ;
	constexpr double epsilon = 1e-9;

	// original format
	vector<double> ref_start = { 0.0,0.0,0.0 };
	vector<double> ref_spacing = { 0.5,0.2,0.3 };
	vector<vector<vector<double>>> ref_data(SX, vector<vector<double>>(SY, vector<double>(SZ, 0.0)));

	for (auto& vv : ref_data)
		for (auto& v : vv)
			for (auto& e : v)
				e = gen();

	Image3D img_ref = Image3D(ref_start, ref_spacing, ref_data);

	vector<double> tar_start = { -0.5,0.3,0.2 };
	vector<double> tar_spacing = { 0.4,0.1,0.4 };
	vector<vector<vector<double>>> tar_data(SX, vector<vector<double>>(SY, vector<double>(SZ, 0.0)));

	for (auto& vv : tar_data)
		for (auto& v : vv)
			for (auto& e : v)
				e = gen();

	Image3D img_tar = Image3D(tar_start, tar_spacing, tar_data);

	SpiralNoRectangleSolver3D solver = SpiralNoRectangleSolver3D(img_ref, img_tar, 0.5f, 0.5f);

	// after optimizations
	vector<double> output_par_unseq(S, 0.0);

	vector<double> ref_doses(S, 0.0);
	vector<double> ref_x(S, 0.0);
	vector<double> ref_y(S, 0.0);
	vector<double> ref_z(S, 0.0);

	vector<double> tar_doses(S, 0.0);
	vector<double> tar_x(S, 0.0);
	vector<double> tar_y(S, 0.0);
	vector<double> tar_z(S, 0.0);

	for (size_t x = 0; x < SX; x++)
		for (size_t y = 0; y < SY; y++)
			for (size_t z = 0; z < SZ; z++)
			{
				auto i = x * SY * SZ + y * SZ + z;
				
				ref_doses[i] = ref_data[x][y][z];
				ref_x[i] = ref_start[0] + ref_spacing[0] * x;
				ref_y[i] = ref_start[1] + ref_spacing[1] * y;
				ref_z[i] = ref_start[2] + ref_spacing[2] * z;

				tar_doses[i] = tar_data[x][y][z];
				tar_x[i] = tar_start[0] + tar_spacing[0] * x;
				tar_y[i] = tar_start[1] + tar_spacing[1] * y;
				tar_z[i] = tar_start[2] + tar_spacing[2] * z;
			}

	local_gamma_index_params<double> params{ 0.5f, 0.5f };

	{
		auto start = high_resolution_clock::now();
		Image3D result = solver.calculateGamma();
		auto end = high_resolution_clock::now();
		BOOST_TEST_MESSAGE("original spiral with took (" << SX << 'x' << SY << 'x' << SZ << " random image) : " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
	}
	{
		auto start = high_resolution_clock::now();
		parallel_unsequenced_gamma_index_implementer<double, 3>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
		parallel_unsequenced_gamma_index_implementer<double, 3>::minimize_pass(
			output_par_unseq.data(), output_par_unseq.data() + S,
			ref_doses.data(),
			array<const double*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
			tar_doses.data(), tar_doses.data() + S,
			array<const double*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
			params
		);
		parallel_unsequenced_gamma_index_implementer<double, 3>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
		auto end = high_resolution_clock::now();
		BOOST_TEST_MESSAGE("parallel unsequenced took (" << SX << 'x' << SY << 'x' << SZ << " random image): " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
	}
}

void expected_specializations_compile_without_errors()
{
	// float32
	{
		// 1
		{
			auto& out = *(iwritable_image<float, 1>*)(nullptr);
			auto& ref = *(irtdose_image<float, 1>*)(nullptr);
			auto& tar = *(irtdose_image<float, 1>*)(nullptr);
			local_gamma_index_params<float> lp{ 0,0 };
			global_gamma_index_params<float> gp{ 0,0 };

			gamma_index(std::execution::seq, out, ref, tar, lp);
			gamma_index(std::execution::seq, out, ref, tar, gp);

			gamma_index(std::execution::unseq, out, ref, tar, lp);
			gamma_index(std::execution::unseq, out, ref, tar, gp);

			gamma_index(std::execution::par, out, ref, tar, lp);
			gamma_index(std::execution::par, out, ref, tar, gp);

			gamma_index(std::execution::par_unseq, out, ref, tar, lp);
			gamma_index(std::execution::par_unseq, out, ref, tar, gp);
		}
		// 2
		{
			auto& out = *(iwritable_image<float, 2>*)(nullptr);
			auto& ref = *(irtdose_image<float, 2>*)(nullptr);
			auto& tar = *(irtdose_image<float, 2>*)(nullptr);
			local_gamma_index_params<float> lp{ 0,0 };
			global_gamma_index_params<float> gp{ 0,0 };

			gamma_index(std::execution::seq, out, ref, tar, lp);
			gamma_index(std::execution::seq, out, ref, tar, gp);

			gamma_index(std::execution::unseq, out, ref, tar, lp);
			gamma_index(std::execution::unseq, out, ref, tar, gp);

			gamma_index(std::execution::par, out, ref, tar, lp);
			gamma_index(std::execution::par, out, ref, tar, gp);

			gamma_index(std::execution::par_unseq, out, ref, tar, lp);
			gamma_index(std::execution::par_unseq, out, ref, tar, gp);
		}
		// 3
		{
			auto& out = *(iwritable_image<float, 3>*)(nullptr);
			auto& ref = *(irtdose_image<float, 3>*)(nullptr);
			auto& tar = *(irtdose_image<float, 3>*)(nullptr);
			local_gamma_index_params<float> lp{ 0,0 };
			global_gamma_index_params<float> gp{ 0,0 };

			gamma_index(std::execution::seq, out, ref, tar, lp);
			gamma_index(std::execution::seq, out, ref, tar, gp);

			gamma_index(std::execution::unseq, out, ref, tar, lp);
			gamma_index(std::execution::unseq, out, ref, tar, gp);

			gamma_index(std::execution::par, out, ref, tar, lp);
			gamma_index(std::execution::par, out, ref, tar, gp);

			gamma_index(std::execution::par_unseq, out, ref, tar, lp);
			gamma_index(std::execution::par_unseq, out, ref, tar, gp);
		}
	}
	// float64
	{
		// 1
		{
			auto& out = *(iwritable_image<double, 1>*)(nullptr);
			auto& ref = *(irtdose_image<double, 1>*)(nullptr);
			auto& tar = *(irtdose_image<double, 1>*)(nullptr);
			local_gamma_index_params<double> lp{ 0,0 };
			global_gamma_index_params<double> gp{ 0,0 };

			gamma_index(std::execution::seq, out, ref, tar, lp);
			gamma_index(std::execution::seq, out, ref, tar, gp);

			gamma_index(std::execution::unseq, out, ref, tar, lp);
			gamma_index(std::execution::unseq, out, ref, tar, gp);

			gamma_index(std::execution::par, out, ref, tar, lp);
			gamma_index(std::execution::par, out, ref, tar, gp);

			gamma_index(std::execution::par_unseq, out, ref, tar, lp);
			gamma_index(std::execution::par_unseq, out, ref, tar, gp);
		}
		// 2
		{
			auto& out = *(iwritable_image<double, 2>*)(nullptr);
			auto& ref = *(irtdose_image<double, 2>*)(nullptr);
			auto& tar = *(irtdose_image<double, 2>*)(nullptr);
			local_gamma_index_params<double> lp{ 0,0 };
			global_gamma_index_params<double> gp{ 0,0 };

			gamma_index(std::execution::seq, out, ref, tar, lp);
			gamma_index(std::execution::seq, out, ref, tar, gp);

			gamma_index(std::execution::unseq, out, ref, tar, lp);
			gamma_index(std::execution::unseq, out, ref, tar, gp);

			gamma_index(std::execution::par, out, ref, tar, lp);
			gamma_index(std::execution::par, out, ref, tar, gp);

			gamma_index(std::execution::par_unseq, out, ref, tar, lp);
			gamma_index(std::execution::par_unseq, out, ref, tar, gp);
		}
		// 3
		{
			auto& out = *(iwritable_image<double, 3>*)(nullptr);
			auto& ref = *(irtdose_image<double, 3>*)(nullptr);
			auto& tar = *(irtdose_image<double, 3>*)(nullptr);
			local_gamma_index_params<double> lp{ 0,0 };
			global_gamma_index_params<double> gp{ 0,0 };

			gamma_index(std::execution::seq, out, ref, tar, lp);
			gamma_index(std::execution::seq, out, ref, tar, gp);

			gamma_index(std::execution::unseq, out, ref, tar, lp);
			gamma_index(std::execution::unseq, out, ref, tar, gp);

			gamma_index(std::execution::par, out, ref, tar, lp);
			gamma_index(std::execution::par, out, ref, tar, gp);

			gamma_index(std::execution::par_unseq, out, ref, tar, lp);
			gamma_index(std::execution::par_unseq, out, ref, tar, gp);
		}
	}
}