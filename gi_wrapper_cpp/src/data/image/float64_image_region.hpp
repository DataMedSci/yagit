#include <math/gamma_index.hpp>
#include <vexcl/vexcl.hpp>
#include <iostream>
#include <chrono>
#include <data/gamma_index.hpp>

#include <boost/proto/core.hpp>
#include <boost/proto/context.hpp>

namespace proto = boost::proto;
using namespace std;
int main()
{
	auto arr =  yagit::core::data::make_index_transform<3>({4,5,7});
	for (size_t i : arr)
		cout << i << ", ";
	auto v = yagit::core::data::index(1, 1, 1);
	int a[] = { 1, 2, 3, 4, 5 };
	cout << endl;
	cout << yagit::core::data::calculate_index<3>({ 1,3,2 }, arr) << endl;
	return 0;
}
/*
using namespace std;



int main()
{
	yagit::core::math::compute::range::gamma_index_context::current_context = std::make_unique<vex::Context>(vex::Filter::DoublePrecision);
	const auto& ctx = *yagit::core::math::compute::range::gamma_index_context::current_context;
	cout << ctx << endl;

	constexpr size_t elements_count = 100000000;
	cout << sizeof(cl_mem) << endl;
	vex::vector<float> X;
	X = X + 2 * X;

	cin.get();

	using namespace simdpp;
	{
		auto output_data = std::make_unique<float[]>(elements_count);
		auto reference_data = std::make_unique<float[]>(elements_count);
		auto target_data = std::make_unique<float[]>(elements_count);
		auto x_data = std::make_unique<float[]>(elements_count);
		auto y_data = std::make_unique<float[]>(elements_count);
		auto z_data = std::make_unique<float[]>(elements_count);
		auto params = yagit::core::math::gamma_index_params<float>{ yagit::core::math::global_gamma_index_params<float>{1,2} };

		cout << "GPU accelerated float" << endl;
		{
			auto start = chrono::high_resolution_clock::now();
			yagit::core::math::gamma_index1D(output_data.get(), reference_data.get(), target_data.get(), x_data.get(), elements_count, params);
			auto end = chrono::high_resolution_clock::now();
			cout << "gamma_index1D with " << elements_count << " elements took: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << "s" << endl;
		}
		{
			auto start = chrono::high_resolution_clock::now();
			yagit::core::math::gamma_index2D(output_data.get(), reference_data.get(), target_data.get(), x_data.get(), y_data.get(), elements_count, params);
			auto end = chrono::high_resolution_clock::now();
			cout << "gamma_index2D with " << elements_count << " elements took: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << "s" << endl;
		}
		{
			auto start = chrono::high_resolution_clock::now();
			yagit::core::math::gamma_index3D(output_data.get(), reference_data.get(), target_data.get(), x_data.get(), y_data.get(), z_data.get(), elements_count, params);
			auto end = chrono::high_resolution_clock::now();
			cout << "gamma_index3D with " << elements_count << " elements took: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << "s" << endl;
		}
	}

	{
		auto output_data = std::make_unique<double[]>(elements_count);
		auto reference_data = std::make_unique<double[]>(elements_count);
		auto target_data = std::make_unique<double[]>(elements_count);
		auto x_data = std::make_unique<double[]>(elements_count);
		auto y_data = std::make_unique<double[]>(elements_count);
		auto z_data = std::make_unique<double[]>(elements_count);
		auto params = yagit::core::math::gamma_index_params<double>{ yagit::core::math::global_gamma_index_params<double>{1,2} };

		cout << "GPU accelerated double" << endl;
		{
			auto start = chrono::high_resolution_clock::now();
			yagit::core::math::gamma_index1D(output_data.get(), reference_data.get(), target_data.get(), x_data.get(), elements_count, params);
			auto end = chrono::high_resolution_clock::now();
			cout << "gamma_index1D with " << elements_count << " elements took: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << "s" << endl;
		}
		{
			auto start = chrono::high_resolution_clock::now();
			yagit::core::math::gamma_index2D(output_data.get(), reference_data.get(), target_data.get(), x_data.get(), y_data.get(), elements_count, params);
			auto end = chrono::high_resolution_clock::now();
			cout << "gamma_index2D with " << elements_count << " elements took: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << "s" << endl;
		}
		{
			auto start = chrono::high_resolution_clock::now();
			yagit::core::math::gamma_index3D(output_data.get(), reference_data.get(), target_data.get(), x_data.get(), y_data.get(), z_data.get(), elements_count, params);
			auto end = chrono::high_resolution_clock::now();
			cout << "gamma_index3D with " << elements_count << " elements took: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << "s" << endl;
		}
	}

	{
		auto output_data = std::make_unique<float[]>(elements_count);
		auto reference_data = std::make_unique<float[]>(elements_count);
		auto target_data = std::make_unique<float[]>(elements_count);
		auto x_data = std::make_unique<float[]>(elements_count);
		auto y_data = std::make_unique<float[]>(elements_count);
		auto z_data = std::make_unique<float[]>(elements_count);
		auto params = yagit::core::math::gamma_index_params<float>{ yagit::core::math::global_gamma_index_params<float>{1,2} };

		cout << "CPU vectorized float" << endl;
		{
			auto start = chrono::high_resolution_clock::now();
			yagit::core::math::gamma_index1D<yagit::core::math::no_acceleration>(output_data.get(), reference_data.get(), target_data.get(), x_data.get(), elements_count, params);
			auto end = chrono::high_resolution_clock::now();
			cout << "gamma_index1D with " << elements_count << " elements took: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << "s" << endl;
		}
		{
			auto start = chrono::high_resolution_clock::now();
			yagit::core::math::gamma_index2D<yagit::core::math::no_acceleration>(output_data.get(), reference_data.get(), target_data.get(), x_data.get(), y_data.get(), elements_count, params);
			auto end = chrono::high_resolution_clock::now();
			cout << "gamma_index2D with " << elements_count << " elements took: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << "s" << endl;
		}
		{
			auto start = chrono::high_resolution_clock::now();
			yagit::core::math::gamma_index3D<yagit::core::math::no_acceleration>(output_data.get(), reference_data.get(), target_data.get(), x_data.get(), y_data.get(), z_data.get(), elements_count, params);
			auto end = chrono::high_resolution_clock::now();
			cout << "gamma_index3D with " << elements_count << " elements took: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << "s" << endl;
		}
	}

	{
		auto output_data = std::make_unique<double[]>(elements_count);
		auto reference_data = std::make_unique<double[]>(elements_count);
		auto target_data = std::make_unique<double[]>(elements_count);
		auto x_data = std::make_unique<double[]>(elements_count);
		auto y_data = std::make_unique<double[]>(elements_count);
		auto z_data = std::make_unique<double[]>(elements_count);
		auto params = yagit::core::math::gamma_index_params<double>{ yagit::core::math::global_gamma_index_params<double>{1,2} };

		cout << "CPU vectorized double" << endl;
		{
			auto start = chrono::high_resolution_clock::now();
			yagit::core::math::gamma_index1D<yagit::core::math::no_acceleration>(output_data.get(), reference_data.get(), target_data.get(), x_data.get(), elements_count, params);
			auto end = chrono::high_resolution_clock::now();
			cout << "gamma_index1D with " << elements_count << " elements took: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << "s" << endl;
		}
		{
			auto start = chrono::high_resolution_clock::now();
			yagit::core::math::gamma_index2D<yagit::core::math::no_acceleration>(output_data.get(), reference_data.get(), target_data.get(), x_data.get(), y_data.get(), elements_count, params);
			auto end = chrono::high_resolution_clock::now();
			cout << "gamma_index2D with " << elements_count << " elements took: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << "s" << endl;
		}
		{
			auto start = chrono::high_resolution_clock::now();
			yagit::core::math::gamma_index3D<yagit::core::math::no_acceleration>(output_data.get(), reference_data.get(), target_data.get(), x_data.get(), y_data.get(), z_data.get(), elements_count, params);
			auto end = chrono::high_resolution_clock::now();
			cout << "gamma_index3D with " << elements_count << " elements took: " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << "s" << endl;
		}
	}

	cin.get();
	/* {
		float* a = nullptr;

		yagit::core::math::gamma_index1D(a, a, a, a, 100, {});
		yagit::core::math::gamma_index2D(a, a, a, a, a, 100, {});
		yagit::core::math::gamma_index3D(a, a, a, a, a, a, 100, {});
	}
	
	{
		double* a = nullptr;

		yagit::core::math::gamma_index1D(a, a, a, a, 100, {});
		yagit::core::math::gamma_index2D(a, a, a, a, a, 100, {});
		yagit::core::math::gamma_index3D(a, a, a, a, a, a, 100, {});
	}

	return 0;
}*/