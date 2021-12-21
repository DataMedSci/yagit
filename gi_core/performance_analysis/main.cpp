#include <math/gamma_index.hpp>
#include <random>
#include <iostream>
#include <image.h>
#include <solver.h>

using namespace yagit::core::math;
using namespace yagit::core::data;
using namespace std;

void test_float()
{
	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<float> uniform_dist(0.0, 10.0f);

	constexpr size_t S = 10000000;
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

	local_gamma_index_params<float> params{ 0.5f, 1.0f };

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
}

void test_double()
{
	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<double> uniform_dist(0.0, 10.0f);

	constexpr size_t S = 1000000;
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

	local_gamma_index_params<double> params{ 0.5f, 1.0f };

	unsequenced_gamma_index_implementer<double, 3>::initialize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
	unsequenced_gamma_index_implementer<double, 3>::minimize_pass(
		output_par_unseq.data(), output_par_unseq.data() + S,
		ref_doses.data(),
		array<const double*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
		tar_doses.data(), tar_doses.data() + S,
		array<const double*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
		params
	);
	unsequenced_gamma_index_implementer<double, 3>::finalize_pass(output_par_unseq.data(), output_par_unseq.data() + S);
}

void test_orig_sr()
{
	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<double> uniform_dist(0.0, 10.0f);
	auto gen = [&]() {return uniform_dist(e1); };

	constexpr size_t SX = 100;
	constexpr size_t SY = 20;
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

	Image3D result = solver.calculateGamma();
}


int main()
{
	string test_type;
	cout << "Select test type [float, double, orig_sr]: ";
	cin >> test_type;

	if (test_type == "float")
	{
		test_float();
	}
	else if (test_type == "double")
	{
		test_double();
	}
	else if(test_type == "orig_sr")
	{
		test_orig_sr();
	}
	
	return 0;
}