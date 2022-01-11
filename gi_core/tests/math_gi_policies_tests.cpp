#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE gi_core_tests
#include <boost/test/unit_test.hpp>

#include <math/classic/basic/gamma_index_range.hpp>
#include <math/classic/basic/openmp/gamma_index_range.hpp>
#include <math/classic/vectorized/gamma_index_range.hpp>
#include <math/classic/vectorized/openmp/gamma_index_range.hpp>

#include <image.h>
#include <solver.h>
#include <../../gi_additions/include/additions.hpp>

#include <random>
#include <chrono>

using namespace yagit::core::math;
using namespace yagit::core::data;
using namespace std;
using namespace std::chrono;
using namespace yagit::core::math::execution;

constexpr size_t SX = 500;
constexpr size_t SY = 1;
constexpr size_t SZ = 1;

constexpr yagit::core::data::sizes<1> img_size_1D = { SX };
constexpr yagit::core::data::sizes<2> img_size_2D = { img_size_1D[0], SY };
constexpr yagit::core::data::sizes<3> img_size_3D = { img_size_2D[0], img_size_2D[1], SZ };

constexpr float epsilonf = 1e-4;
constexpr double epsilon = 1e-9;

constexpr yagit::core::data::image_position_t<float, 1> ref_pos_1Df = { 2.0f };
constexpr yagit::core::data::image_position_t<float, 2> ref_pos_2Df = { ref_pos_1Df[0], 3.0f};
constexpr yagit::core::data::image_position_t<float, 3> ref_pos_3Df = { ref_pos_2Df[0], ref_pos_2Df[1], 1.0f };
constexpr yagit::core::data::image_position_t<double, 1> ref_pos_1D = { ref_pos_1Df[0] };
constexpr yagit::core::data::image_position_t<double, 2> ref_pos_2D = { ref_pos_1D[0], ref_pos_2Df[1] };
constexpr yagit::core::data::image_position_t<double, 3> ref_pos_3D = { ref_pos_2D[0], ref_pos_2D[1], ref_pos_3Df[2] };

constexpr yagit::core::data::uniform_image_spacing_t<float, 1> ref_spac_1Df = { 0.3f };
constexpr yagit::core::data::uniform_image_spacing_t<float, 2> ref_spac_2Df = { ref_spac_1Df[0], 0.25f };
constexpr yagit::core::data::uniform_image_spacing_t<float, 3> ref_spac_3Df = { ref_spac_2Df[0], ref_spac_2Df[1], 0.4f };
constexpr yagit::core::data::uniform_image_spacing_t<double, 1> ref_spac_1D = { ref_spac_1Df[0] };
constexpr yagit::core::data::uniform_image_spacing_t<double, 2> ref_spac_2D = { ref_spac_1D[0], ref_spac_2Df[1] };
constexpr yagit::core::data::uniform_image_spacing_t<double, 3> ref_spac_3D = { ref_spac_2D[0], ref_spac_2D[1], ref_spac_3Df[2] };

constexpr yagit::core::data::image_position_t<float, 1> tar_pos_1Df = { 0.0f };
constexpr yagit::core::data::image_position_t<float, 2> tar_pos_2Df = { tar_pos_1Df[0], 0.0f };
constexpr yagit::core::data::image_position_t<float, 3> tar_pos_3Df = { tar_pos_2Df[0], tar_pos_2Df[1], 0.0f };
constexpr yagit::core::data::image_position_t<double, 1> tar_pos_1D = { tar_pos_1Df[0] };
constexpr yagit::core::data::image_position_t<double, 2> tar_pos_2D = { tar_pos_1D[0], tar_pos_2Df[1] };
constexpr yagit::core::data::image_position_t<double, 3> tar_pos_3D = { tar_pos_2D[0], tar_pos_2D[1], tar_pos_3Df[2] };

constexpr yagit::core::data::uniform_image_spacing_t<float, 1> tar_spac_1Df = { 0.4f };
constexpr yagit::core::data::uniform_image_spacing_t<float, 2> tar_spac_2Df = { tar_spac_1Df[0], 0.2f };
constexpr yagit::core::data::uniform_image_spacing_t<float, 3> tar_spac_3Df = { tar_spac_2Df[0], tar_spac_2Df[1], 0.5f };
constexpr yagit::core::data::uniform_image_spacing_t<double, 1> tar_spac_1D = { tar_spac_1Df[0] };
constexpr yagit::core::data::uniform_image_spacing_t<double, 2> tar_spac_2D = { tar_spac_1D[0], tar_spac_2Df[1] };
constexpr yagit::core::data::uniform_image_spacing_t<double, 3> tar_spac_3D = { tar_spac_2D[0], tar_spac_2D[1], tar_spac_3Df[2] };

constexpr float low_dose_limitf = 0.0f;
constexpr float high_dose_limitf = 6.0f;
constexpr double low_dose_limit = low_dose_limitf;
constexpr double high_dose_limit = high_dose_limitf;

constexpr float dose_cutofff = 0.05f;
constexpr double dose_cutoff = dose_cutofff;

constexpr local_gamma_index_params<float> local_paramsf = local_gamma_index_params<float>(0.02f, 2.0f);
constexpr global_gamma_index_params<float> global_paramsf = global_gamma_index_params<float>(high_dose_limit, 2.0f);
constexpr local_gamma_index_params<double> local_params = local_gamma_index_params<double>(0.02f, 2.0f);
constexpr global_gamma_index_params<double> global_params = global_gamma_index_params<double>(high_dose_limit, 2.0f);

Image1D convert_to_image(
	const yagit::core::data::const_view<double>& image_doses_data,
	const yagit::core::data::sizes<1>& image_size,
	const yagit::core::data::image_position_t<double, 1>& image_pos,
	const yagit::core::data::uniform_image_spacing_t<double, 1>& image_spac)
{
	std::vector<double> img_data(image_size[0], 0.0);
	for (size_t x = 0; x < image_size[0]; x++)
		img_data[x] = image_doses_data[x];
	return Image1D(
		std::vector<double>(std::begin(image_pos), std::end(image_pos)),
		std::vector<double>(std::begin(image_spac), std::end(image_spac)),
		img_data
	);
}

Image2D convert_to_image(
	const yagit::core::data::const_view<double>& image_doses_data,
	const yagit::core::data::sizes<2>& image_size,
	const yagit::core::data::image_position_t<double, 2>& image_pos,
	const yagit::core::data::uniform_image_spacing_t<double, 2>& image_spac)
{
	std::vector<std::vector<double>> img_data(image_size[1], std::vector<double>(image_size[0], 0.0));
	for (size_t y = 0; y < image_size[1]; y++)
		for (size_t x = 0; x < image_size[0]; x++)
			img_data[y][x] = image_doses_data[x * image_size[1] + y];
	return Image2D(
		std::vector<double>(std::begin(image_pos), std::end(image_pos)),
		std::vector<double>(std::begin(image_spac), std::end(image_spac)),
		img_data
	);
}

Image3D convert_to_image(
	const yagit::core::data::const_view<double>& image_doses_data,
	const yagit::core::data::sizes<3>& image_size,
	const yagit::core::data::image_position_t<double, 3>& image_pos,
	const yagit::core::data::uniform_image_spacing_t<double, 3>& image_spac)
{
	std::vector<std::vector<std::vector<double>>> img_data(image_size[2], std::vector<std::vector<double>>(image_size[1], std::vector<double>(image_size[0], 0.0)));
	for (size_t z = 0; z < image_size[2]; z++)
		for (size_t y = 0; y < image_size[1]; y++)
			for (size_t x = 0; x < image_size[0]; x++)
				img_data[z][y][x] = image_doses_data[(x * image_size[1] + y) * image_size[2] + z];
	return Image3D(
		std::vector<double>(std::begin(image_pos), std::end(image_pos)),
		std::vector<double>(std::begin(image_spac), std::end(image_spac)),
		img_data
	);
}

void convert_to_raw_data(
	const yagit::core::data::view<double>& output,
	const Image1D& img)
{
	auto shape = img.getReversedShape();
	for (size_t x = 0; x < shape[0]; x++)
		output[x] = img.data()[x];
}

void convert_to_raw_data(
	const yagit::core::data::view<double>& output,
	const Image2D& img)
{
	auto shape = img.getReversedShape();
	for (size_t y = 0; y < shape[1]; y++)
		for (size_t x = 0; x < shape[0]; x++)
			output[x*shape[1]+y] = img.data()[y][x];
}

void convert_to_raw_data(
	const yagit::core::data::view<double>& output,
	const Image3D& img)
{
	auto shape = img.getReversedShape();
	for (size_t z = 0; z < shape[2]; z++)
		for (size_t y = 0; y < shape[1]; y++)
			for (size_t x = 0; x < shape[0]; x++)
				output[(x * shape[1] + y) * shape[2] + z] = img.data()[z][y][x];
}

template<typename ElementType, size_t Dimensions>
struct relative_classic_policies_test_data
{
	relative_classic_policies_test_data(
		const yagit::core::data::sizes<Dimensions>& image_size,
		const yagit::core::data::image_position_t<ElementType, Dimensions>& ref_pos,
		const yagit::core::data::uniform_image_spacing_t<ElementType, Dimensions>& ref_spac,
		const yagit::core::data::image_position_t<ElementType, Dimensions>& tar_pos,
		const yagit::core::data::uniform_image_spacing_t<ElementType, Dimensions>& tar_spac)
		: output_seq(total_size(image_size), static_cast<ElementType>(0.0))
		, output_unseq(total_size(image_size), static_cast<ElementType>(0.0))
		, output_par(total_size(image_size), static_cast<ElementType>(0.0))
		, output_par_unseq(total_size(image_size), static_cast<ElementType>(0.0))
		, ref_doses(total_size(image_size), static_cast<ElementType>(0.0))
		, tar_doses(total_size(image_size), static_cast<ElementType>(0.0))
		, ref_pos(ref_pos)
		, ref_spac(ref_spac)
		, tar_pos(tar_pos)
		, tar_spac(tar_spac)
		, ref_size(image_size)
		, tar_size(image_size)

	{
		for (size_t i = 0; i < Dimensions; i++)
		{
			ref_coords[i] = vector<ElementType>(total_size(image_size), static_cast<ElementType>(0.0));
			tar_coords[i] = vector<ElementType>(total_size(image_size), static_cast<ElementType>(0.0));
		}
	}

	vector<ElementType> output_seq;
	vector<ElementType> output_unseq;
	vector<ElementType> output_par;
	vector<ElementType> output_par_unseq;

	vector<ElementType> ref_doses;
	std::array<vector<ElementType>, Dimensions> ref_coords;
	yagit::core::data::sizes<Dimensions> ref_size;
	yagit::core::data::image_position_t<ElementType, Dimensions> ref_pos;
	yagit::core::data::uniform_image_spacing_t<ElementType, Dimensions> ref_spac;

	vector<ElementType> tar_doses;
	std::array<vector<ElementType>, Dimensions> tar_coords;
	yagit::core::data::sizes<Dimensions> tar_size;
	yagit::core::data::image_position_t<ElementType, Dimensions> tar_pos;
	yagit::core::data::uniform_image_spacing_t<ElementType, Dimensions> tar_spac;

	auto doses_ref()
	{
		return yagit::core::data::view<ElementType>{ref_doses.data(), ref_doses.data() + ref_doses.size()};
	}
	auto doses_tar()
	{
		return yagit::core::data::view<ElementType>{tar_doses.data(), tar_doses.data() + tar_doses.size()};
	}

	auto coords_ref()
	{
		std::array<yagit::core::data::view<ElementType>, Dimensions> views;

		std::transform(ref_coords.begin(), ref_coords.end(), views.begin(), [](auto& coords_vector)
			{
				return yagit::core::data::view<ElementType>{coords_vector.data(), coords_vector.data() + coords_vector.size()};
			});
		return views;
	}
	auto coords_tar()
	{
		std::array<yagit::core::data::view<ElementType>, Dimensions> views;

		std::transform(tar_coords.begin(), tar_coords.end(), views.begin(), [](auto& coords_vector)
			{
				return yagit::core::data::view<ElementType>{coords_vector.data(), coords_vector.data() + coords_vector.size()};
			});
		return views;
	}

	auto ref_input()
	{
		return yagit::core::math::input_uniform_image_view<ElementType, Dimensions>{
			.doses = { ref_doses.data(), ref_doses.data() + ref_doses.size() },
			.image_size = ref_size,
			.image_metadata = { .image_position = ref_pos, .image_spacing = ref_spac }
		};
	}
	auto tar_input()
	{
		return yagit::core::math::input_uniform_image_view<ElementType, Dimensions>{
			.doses = { tar_doses.data(), tar_doses.data() + tar_doses.size() },
			.image_size = tar_size,
			.image_metadata = { .image_position = tar_pos, .image_spacing = tar_spac }
		};
	}
	auto classic_params()
	{
		algorithm_version::classic::parameters<ElementType, Dimensions> params;
		std::transform(ref_coords.begin(), ref_coords.end(), params.reference_image_coordinates.begin(), [](auto& coords_vector)
			{
				return yagit::core::data::const_view<ElementType>{coords_vector.data(), coords_vector.data() + coords_vector.size()};
			});
		std::transform(tar_coords.begin(), tar_coords.end(), params.target_image_coordinates.begin(), [](auto& coords_vector)
			{
				return yagit::core::data::const_view<ElementType>{coords_vector.data(), coords_vector.data() + coords_vector.size()};
			});
		return params;
	}
	auto output(sequenced_policy)
	{
		return yagit::core::math::output_image_view<ElementType, Dimensions>{
			.output = { output_seq.data(), output_seq.data() + output_seq.size() },
			.image_size = ref_size
		};
	}
	auto output(unsequenced_policy)
	{
		return yagit::core::math::output_image_view<ElementType, Dimensions>{
			.output = { output_unseq.data(), output_unseq.data() + output_unseq.size() },
			.image_size = ref_size
		};
	}
	auto output(parallel_policy)
	{
		return yagit::core::math::output_image_view<ElementType, Dimensions>{
			.output = { output_par.data(), output_par.data() + output_par.size() },
			.image_size = ref_size
		};
	}
	auto output(parallel_unsequenced_policy)
	{
		return yagit::core::math::output_image_view<ElementType, Dimensions>{
			.output = { output_par_unseq.data(), output_par_unseq.data() + output_par_unseq.size() },
			.image_size = ref_size
		};
	}
};

template<typename ElementType, size_t Dimensions>
relative_classic_policies_test_data<ElementType, Dimensions> generate_data(
	const yagit::core::data::sizes<Dimensions>& image_size,
	const yagit::core::data::image_position_t<ElementType, Dimensions>& ref_pos,
	const yagit::core::data::uniform_image_spacing_t<ElementType, Dimensions>& ref_spac,
	const yagit::core::data::image_position_t<ElementType, Dimensions>& tar_pos,
	const yagit::core::data::uniform_image_spacing_t<ElementType, Dimensions>& tar_spac,
	ElementType low_dose, ElementType high_dose
)
{
	relative_classic_policies_test_data<ElementType, Dimensions> data_holder(
		image_size,
		ref_pos, ref_spac,
		tar_pos, tar_spac
	);

	yagit::additions::generate_random_doses(
		yagit::core::data::view<ElementType>{ data_holder.ref_doses.data(), data_holder.ref_doses.data() + data_holder.ref_doses.size() },
		low_dose, high_dose);
	yagit::additions::generate_random_doses(
		yagit::core::data::view<ElementType>{ data_holder.tar_doses.data(), data_holder.tar_doses.data() + data_holder.tar_doses.size() },
		low_dose, high_dose);
	
	yagit::additions::generate_coordinates(data_holder.coords_ref(), image_size, ref_pos, ref_spac);
	yagit::additions::generate_coordinates(data_holder.coords_tar(), image_size, tar_pos, tar_spac);

	return data_holder;
}

template<typename ElementType, size_t Dimensions>
relative_classic_policies_test_data<ElementType, Dimensions> generate_data_and_run_classic_tests(
	const yagit::core::data::sizes<Dimensions>& image_size,
	const yagit::core::data::image_position_t<ElementType, Dimensions>& ref_pos,
	const yagit::core::data::uniform_image_spacing_t<ElementType, Dimensions>& ref_spac,
	const yagit::core::data::image_position_t<ElementType, Dimensions>& tar_pos,
	const yagit::core::data::uniform_image_spacing_t<ElementType, Dimensions>& tar_spac,
	const global_gamma_index_params<ElementType>& global_params,
	const local_gamma_index_params<ElementType>& local_params,
	ElementType low_dose, ElementType high_dose,
	ElementType epsilon,
	ElementType dcu
)
{
	relative_classic_policies_test_data<ElementType, Dimensions> test_run_data
		= generate_data<ElementType, Dimensions>(image_size, ref_pos, ref_spac, tar_pos, tar_spac, low_dose, high_dose);
	
	auto params_run = [&](auto&& params)
	{
		{
			BOOST_TEST_MESSAGE("\t\t\tsequenced");
			{
				auto start = high_resolution_clock::now();
				gamma_index(
					algorithm_version::classic{},
					sequenced_policy{},
					test_run_data.output(sequenced_policy{}),
					test_run_data.ref_input(),
					test_run_data.tar_input(),
					test_run_data.classic_params(),
					params
				);
				auto end = high_resolution_clock::now();
				BOOST_TEST_MESSAGE("\t\t\t\tTook: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
			}
			BOOST_TEST_MESSAGE("\t\t\tparallel");
			{
				auto start = high_resolution_clock::now();
				gamma_index(
					algorithm_version::classic{},
					parallel_policy{},
					test_run_data.output(parallel_policy{}),
					test_run_data.ref_input(),
					test_run_data.tar_input(),
					test_run_data.classic_params(),
					params
				);
				auto end = high_resolution_clock::now();
				BOOST_TEST_MESSAGE("\t\t\t\tTook: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
			}
			BOOST_TEST_MESSAGE("\t\t\tunsequenced");
			{
				auto start = high_resolution_clock::now();
				gamma_index(
					algorithm_version::classic{},
					unsequenced_policy{},
					test_run_data.output(unsequenced_policy{}),
					test_run_data.ref_input(),
					test_run_data.tar_input(),
					test_run_data.classic_params(),
					params
				);
				auto end = high_resolution_clock::now();
				BOOST_TEST_MESSAGE("\t\t\t\tTook: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
			}
			BOOST_TEST_MESSAGE("\t\t\tparallel_unsequenced");
			{
				auto start = high_resolution_clock::now();
				gamma_index(
					algorithm_version::classic{},
					parallel_unsequenced_policy{},
					test_run_data.output(parallel_unsequenced_policy{}),
					test_run_data.ref_input(),
					test_run_data.tar_input(),
					test_run_data.classic_params(),
					params
				);
				auto end = high_resolution_clock::now();
				BOOST_TEST_MESSAGE("\t\t\t\tTook: " << duration_cast<nanoseconds>(end - start).count() / 1e9 << 's');
			}
		}
	};

	auto run_comparison = [&]()
	{
		auto comp = [&](auto a, auto b) {return (a <= static_cast<ElementType>(1) && b <= static_cast<ElementType>(1)) || (std::abs(a - b) < epsilon) || b < a; };
		auto reference_output_b = test_run_data.output_seq.begin();
		auto reference_output_e = test_run_data.output_seq.end();

		auto unseq_equal_to_seq =
			yagit::additions::avg_abs_gi_difference(
				yagit::core::data::make_const_view(test_run_data.output_seq),
				yagit::core::data::make_const_view(test_run_data.output_unseq)
			);
		auto par_equal_to_seq =
			yagit::additions::avg_abs_gi_difference(
				yagit::core::data::make_const_view(test_run_data.output_seq),
				yagit::core::data::make_const_view(test_run_data.output_par)
			);
		auto par_unseq_equal_to_seq =
			yagit::additions::avg_abs_gi_difference(
				yagit::core::data::make_const_view(test_run_data.output_seq),
				yagit::core::data::make_const_view(test_run_data.output_par_unseq)
			);

		BOOST_CHECK(unseq_equal_to_seq < 1.0f);
		BOOST_TEST_MESSAGE("avg abs gi difference = " << unseq_equal_to_seq);
		BOOST_CHECK(par_equal_to_seq < 1.0f);
		BOOST_TEST_MESSAGE("avg abs gi difference = " << par_equal_to_seq);
		BOOST_CHECK(par_unseq_equal_to_seq < 1.0f);
		BOOST_TEST_MESSAGE("avg abs gi difference = " << par_unseq_equal_to_seq);
	};

	BOOST_TEST_MESSAGE("\t\tGLOBAL unfiltered");
	params_run(global_params);
	BOOST_TEST_MESSAGE("\t\tOUTPUT_COMPARISON");
	run_comparison();
	BOOST_TEST_MESSAGE("\t\tLOCAL unfiltered");
	params_run(local_params);
	BOOST_TEST_MESSAGE("\t\tOUTPUT_COMPARISON");
	run_comparison();

	yagit::additions::filter_image_data_based_on_dose_cutoff(test_run_data.doses_ref(), dcu);
	yagit::additions::filter_image_data_based_on_dose_cutoff(test_run_data.doses_tar(), dcu);

	BOOST_TEST_MESSAGE("\t\tGLOBAL filtered");
	params_run(global_params);
	BOOST_TEST_MESSAGE("\t\tOUTPUT_COMPARISON");
	run_comparison();
	BOOST_TEST_MESSAGE("\t\tLOCAL filtered");
	params_run(local_params);
	BOOST_TEST_MESSAGE("\t\tOUTPUT_COMPARISON");
	run_comparison();

	return test_run_data;
}

template<typename F1D, typename F2D, typename F3D>
array<double, 3> run_spiral_tests
(
	const relative_classic_policies_test_data<double, 1>& in1D,
	const relative_classic_policies_test_data<double, 2>& in2D,
	const relative_classic_policies_test_data<double, 3>& in3D,
	const yagit::core::data::view<double>& out_1D,
	const yagit::core::data::view<double>& out_2D,
	const yagit::core::data::view<double>& out_3D,
	F1D f1d, F2D f2d, F3D f3d
)
{
	array<double, 3> timings;
	BOOST_TEST_MESSAGE("\t\t1D");
	{
		auto ref_im = convert_to_image(
			yagit::core::data::const_view<double>{std::data(in1D.ref_doses), std::data(in1D.ref_doses) + std::size(in1D.ref_doses)},
			in1D.ref_size,
			in1D.ref_pos,
			in1D.ref_spac
		);
		auto tar_im = convert_to_image(
			yagit::core::data::const_view<double>{std::data(in1D.tar_doses), std::data(in1D.tar_doses) + std::size(in1D.tar_doses)},
			in1D.tar_size,
			in1D.tar_pos,
			in1D.tar_spac
		);
		auto start = high_resolution_clock::now();
		auto out_im = f1d(ref_im, tar_im);
		auto end = high_resolution_clock::now();
		timings[0] = duration_cast<nanoseconds>(end - start).count() / 1e9;
		BOOST_TEST_MESSAGE("\t\t\tTook: " << timings[0] << 's');
		convert_to_raw_data(out_1D, out_im);
	}
	BOOST_TEST_MESSAGE("\t\t2D");
	{
		auto ref_im = convert_to_image(
			yagit::core::data::const_view<double>{std::data(in2D.ref_doses), std::data(in2D.ref_doses) + std::size(in2D.ref_doses)},
			in2D.ref_size,
			in2D.ref_pos,
			in2D.ref_spac
		);
		auto tar_im = convert_to_image(
			yagit::core::data::const_view<double>{std::data(in2D.tar_doses), std::data(in2D.tar_doses) + std::size(in2D.tar_doses)},
			in2D.tar_size,
			in2D.tar_pos,
			in2D.tar_spac
		);
		auto start = high_resolution_clock::now();
		auto out_im = f2d(ref_im, tar_im);
		auto end = high_resolution_clock::now();
		timings[1] = duration_cast<nanoseconds>(end - start).count() / 1e9;
		BOOST_TEST_MESSAGE("\t\t\tTook: " << timings[1] << 's');
		convert_to_raw_data(out_2D, out_im);
	}
	BOOST_TEST_MESSAGE("\t\t3D");
	{
		auto ref_im = convert_to_image(
			yagit::core::data::const_view<double>{std::data(in3D.ref_doses), std::data(in3D.ref_doses) + std::size(in3D.ref_doses)},
			in3D.ref_size,
			in3D.ref_pos,
			in3D.ref_spac
		);
		auto tar_im = convert_to_image(
			yagit::core::data::const_view<double>{std::data(in3D.tar_doses), std::data(in3D.tar_doses) + std::size(in3D.tar_doses)},
			in3D.tar_size,
			in3D.tar_pos,
			in3D.tar_spac
		);
		auto start = high_resolution_clock::now();
		auto out_im = f3d(ref_im, tar_im);
		auto end = high_resolution_clock::now();
		timings[2] = duration_cast<nanoseconds>(end - start).count() / 1e9;
		BOOST_TEST_MESSAGE("\t\t\tTook: " << timings[2] << 's');
		convert_to_raw_data(out_3D, out_im);
	}
	return timings;
}

/*
BOOST_AUTO_TEST_CASE(policies_relative_correctness_float)
{
	BOOST_TEST_MESSAGE("------------ policies_relative_correctness_float ------------");
	BOOST_TEST_MESSAGE("\t1D");
	generate_data_and_run_classic_tests<float, 1>(
		img_size_1D,
		ref_pos_1Df, ref_spac_1Df,
		tar_pos_1Df, tar_spac_1Df,
		global_paramsf, local_paramsf,
		low_dose_limitf, high_dose_limitf, epsilonf, dose_cutofff
		);
	BOOST_TEST_MESSAGE("\t2D");
	generate_data_and_run_classic_tests<float, 2>(
		img_size_2D,
		ref_pos_2Df, ref_spac_2Df,
		tar_pos_2Df, tar_spac_2Df,
		global_paramsf, local_paramsf,
		low_dose_limitf, high_dose_limitf, epsilonf, dose_cutofff
		);
	BOOST_TEST_MESSAGE("\t3D");
	generate_data_and_run_classic_tests<float, 3>(
		img_size_3D,
		ref_pos_3Df, ref_spac_3Df,
		tar_pos_3Df, tar_spac_3Df,
		global_paramsf, local_paramsf,
		low_dose_limitf, high_dose_limitf, epsilonf, dose_cutofff
		);
}

BOOST_AUTO_TEST_CASE(policies_relative_correctness_double)
{
	BOOST_TEST_MESSAGE("------------ policies_relative_correctness_double ------------");
	BOOST_TEST_MESSAGE("\t1D");
	generate_data_and_run_classic_tests<double, 1>(
		img_size_1D,
		ref_pos_1D, ref_spac_1D,
		tar_pos_1D, tar_spac_1D,
		global_params, local_params,
		low_dose_limit, high_dose_limit, epsilon, dose_cutoff
		);
	BOOST_TEST_MESSAGE("\t2D");
	generate_data_and_run_classic_tests<double, 2>(
		img_size_2D,
		ref_pos_2D, ref_spac_2D,
		tar_pos_2D, tar_spac_2D,
		global_params, local_params,
		low_dose_limit, high_dose_limit, epsilon, dose_cutoff
		);
	BOOST_TEST_MESSAGE("\t3D");
	generate_data_and_run_classic_tests<double, 3>(
		img_size_3D,
		ref_pos_3D, ref_spac_3D,
		tar_pos_3D, tar_spac_3D,
		global_params, local_params,
		low_dose_limit, high_dose_limit, epsilon, dose_cutoff
		);
}
*/
template<typename ElementType, size_t Dimensions, typename ParamsType>
array<double, 4> run_vectorized_gi_on_data(relative_classic_policies_test_data<ElementType, Dimensions>& data, const ParamsType& params)
{
	array<double, 4> timings;

	//seq
	{
		auto start = high_resolution_clock::now();
		gamma_index(
			algorithm_version::classic{},
			sequenced_policy{},
			data.output(sequenced_policy{}),
			data.ref_input(),
			data.tar_input(),
			data.classic_params(),
			params
		);
		auto end = high_resolution_clock::now();
		timings[0] = duration_cast<nanoseconds>(end - start).count() / 1e9;
	}
	//par
	{
		auto start = high_resolution_clock::now();
		gamma_index(
			algorithm_version::classic{},
			parallel_policy{},
			data.output(parallel_policy{}),
			data.ref_input(),
			data.tar_input(),
			data.classic_params(),
			params
		);
		auto end = high_resolution_clock::now();
		timings[1] = duration_cast<nanoseconds>(end - start).count() / 1e9;
	}
	//unseq
	{
		auto start = high_resolution_clock::now();
		gamma_index(
			algorithm_version::classic{},
			unsequenced_policy{},
			data.output(unsequenced_policy{}),
			data.ref_input(),
			data.tar_input(),
			data.classic_params(),
			params
		);
		auto end = high_resolution_clock::now();
		timings[2] = duration_cast<nanoseconds>(end - start).count() / 1e9;
	}
	//par unseq
	{
		auto start = high_resolution_clock::now();
		gamma_index(
			algorithm_version::classic{},
			parallel_unsequenced_policy{},
			data.output(parallel_unsequenced_policy{}),
			data.ref_input(),
			data.tar_input(),
			data.classic_params(),
			params
		);
		auto end = high_resolution_clock::now();
		timings[3] = duration_cast<nanoseconds>(end - start).count() / 1e9;
	}

	return timings;
}

template<size_t BinsCount, typename ElementType>
void make_and_print_histogram(const yagit::core::data::const_view<ElementType>& output, ElementType eps)
{
	auto image_min = std::max(static_cast<ElementType>(1), yagit::additions::min(output));
	auto image_max = std::max(static_cast<ElementType>(1), yagit::additions::max(output) + eps);
	auto bin_spacing = (image_max - image_min) / BinsCount;
	array<ElementType, BinsCount + 2> delimeters;
	delimeters[0] = static_cast<ElementType>(0);
	delimeters[1] = static_cast<ElementType>(1);
	delimeters.back() = image_max;
	array<size_t, BinsCount + 3> bins;
	for (size_t i = 2; i < BinsCount + 2; i++)
	{
		delimeters[i] = image_min + i * bin_spacing;
	}
	yagit::additions::make_histogram<true>(
		bins.data(),
		output,
		delimeters.data(), delimeters.data() + delimeters.size());
	for (size_t i = 0; i < BinsCount + 1; i++)
	{
		BOOST_TEST_MESSAGE("Range<" << delimeters[i] << ", " << delimeters[i + 1] << ">: " << bins[i]);
	}
	BOOST_TEST_MESSAGE("Not in delimeters: " << bins[BinsCount + 1]);
	BOOST_TEST_MESSAGE("NaNs: " << bins[BinsCount + 2]);
}

BOOST_AUTO_TEST_CASE(comparison_against_spiral)
{
	BOOST_TEST_MESSAGE("------------ comparison_against_spiral ------------");
	BOOST_TEST_MESSAGE("Generating comparison data...");

	auto data1D = generate_data<double, 1>(img_size_1D, ref_pos_1D, ref_spac_1D, tar_pos_1D, tar_spac_1D, low_dose_limit, high_dose_limit);
	auto data2D = generate_data<double, 2>(img_size_2D, ref_pos_2D, ref_spac_2D, tar_pos_2D, tar_spac_2D, low_dose_limit, high_dose_limit);
	auto data3D = generate_data<double, 3>(img_size_3D, ref_pos_3D, ref_spac_3D, tar_pos_3D, tar_spac_3D, low_dose_limit, high_dose_limit);

	auto make_comparison_local = [&]()
	{
		BOOST_TEST_MESSAGE("\t\t1D");
		auto classic_vectorized_timings_1D = run_vectorized_gi_on_data(data1D, local_params);
		BOOST_TEST_MESSAGE("\t\t\tTook: " << classic_vectorized_timings_1D[3] << 's');
		BOOST_TEST_MESSAGE("\t\t2D");
		auto classic_vectorized_timings_2D = run_vectorized_gi_on_data(data2D, local_params);
		BOOST_TEST_MESSAGE("\t\t\tTook: " << classic_vectorized_timings_2D[3] << 's');
		BOOST_TEST_MESSAGE("\t\t3D");
		auto classic_vectorized_timings_3D = run_vectorized_gi_on_data(data3D, local_params);
		BOOST_TEST_MESSAGE("\t\t\tTook: " << classic_vectorized_timings_3D[3] << 's');

		std::vector<double> out1D(total_size(data1D.ref_size));
		std::vector<double> out2D(total_size(data2D.ref_size));
		std::vector<double> out3D(total_size(data3D.ref_size));

		auto spiral_timings123D = run_spiral_tests(
			data1D, data2D, data3D,
			yagit::core::data::view<double>{std::data(out1D), std::data(out1D) + std::size(out1D)},
			yagit::core::data::view<double>{std::data(out2D), std::data(out2D) + std::size(out2D)},
			yagit::core::data::view<double>{std::data(out3D), std::data(out3D) + std::size(out3D)},
			[](auto&& ref1D, auto&& tar1D)
			{
				SpiralRectangleSolver1D solver(ref1D, tar1D, local_params.reference_dose_percentage(), local_params.distance_to_agreement_normalization());
				solver.setLocal(true);
				return solver.calculateGamma();
			},
			[](auto&& ref2D, auto&& tar2D)
			{
				SpiralRectangleSolver2D solver(ref2D, tar2D, local_params.reference_dose_percentage(), local_params.distance_to_agreement_normalization());
				solver.setLocal(true);
				return solver.calculateGamma();
			},
			[](auto&& ref3D, auto&& tar3D)
			{
				SpiralRectangleSolver3D solver(ref3D, tar3D, local_params.reference_dose_percentage(), local_params.distance_to_agreement_normalization());
				solver.setLocal(true);
				return solver.calculateGamma();
			}
			);

		// do compare
		auto avg_abs_diff_1D = yagit::additions::avg_abs_gi_difference(
			yagit::core::data::make_const_view(data1D.output_par_unseq),
			yagit::core::data::make_const_view(out1D)
		);
		auto avg_abs_diff_2D = yagit::additions::avg_abs_gi_difference(
			yagit::core::data::make_const_view(data2D.output_par_unseq),
			yagit::core::data::make_const_view(out2D)
		);
		auto avg_abs_diff_3D = yagit::additions::avg_abs_gi_difference(
			yagit::core::data::make_const_view(data3D.output_par_unseq),
			yagit::core::data::make_const_view(out3D)
		);

		auto s1D_gipr = yagit::additions::active_voxels_passing_rate(
			yagit::core::data::make_const_view(out1D)
		);
		auto s2D_gipr = yagit::additions::active_voxels_passing_rate(
			yagit::core::data::make_const_view(out2D)
		);
		auto s3D_gipr = yagit::additions::active_voxels_passing_rate(
			yagit::core::data::make_const_view(out3D)
		);

		auto s1D_avg_gi = yagit::additions::avg(
			yagit::core::data::make_const_view(out1D)
		);
		auto s2D_avg_gi = yagit::additions::avg(
			yagit::core::data::make_const_view(out2D)
		);
		auto s3D_avg_gi = yagit::additions::avg(
			yagit::core::data::make_const_view(out3D)
		);

		auto cpu1D_gipr = yagit::additions::active_voxels_passing_rate(
			yagit::core::data::make_const_view(data1D.output_par_unseq)
		);
		auto cpu2D_gipr = yagit::additions::active_voxels_passing_rate(
			yagit::core::data::make_const_view(data2D.output_par_unseq)
		);
		auto cpu3D_gipr = yagit::additions::active_voxels_passing_rate(
			yagit::core::data::make_const_view(data3D.output_par_unseq)
		);

		auto cpu1D_avg_gi = yagit::additions::avg(
			yagit::core::data::make_const_view(data1D.output_par_unseq)
		);
		auto cpu2D_avg_gi = yagit::additions::avg(
			yagit::core::data::make_const_view(data2D.output_par_unseq)
		);
		auto cpu3D_avg_gi = yagit::additions::avg(
			yagit::core::data::make_const_view(data3D.output_par_unseq)
		);

		BOOST_TEST_MESSAGE("1D spiral vs par_unseq: " << avg_abs_diff_1D << " spiral GIPR: " << s1D_gipr << " classic par_unseq GIPR: " << cpu1D_gipr);
		BOOST_TEST_MESSAGE("1D spiral avg GI : " << s1D_avg_gi << " classic par_unseq avg GI : " << cpu1D_avg_gi);
		BOOST_TEST_MESSAGE("1D spiral output histogram:");
		make_and_print_histogram<20>(yagit::core::data::make_const_view(out1D), epsilon);
		BOOST_TEST_MESSAGE("1D classic par_unseq output histogram:");
		make_and_print_histogram<20>(yagit::core::data::make_const_view(data1D.output_par_unseq), epsilon);
		BOOST_TEST_MESSAGE("2D spiral vs par_unseq: " << avg_abs_diff_2D << " spiral GIPR: " << s2D_gipr << " classic par_unseq GIPR: " << cpu2D_gipr);
		BOOST_TEST_MESSAGE("2D spiral avg GI : " << s2D_avg_gi << " classic par_unseq avg GI : " << cpu2D_avg_gi);
		BOOST_TEST_MESSAGE("2D spiral output histogram:");
		make_and_print_histogram<20>(yagit::core::data::make_const_view(out2D), epsilon);
		BOOST_TEST_MESSAGE("2D classic par_unseq output histogram:");
		make_and_print_histogram<20>(yagit::core::data::make_const_view(data2D.output_par_unseq), epsilon);
		BOOST_TEST_MESSAGE("3D spiral vs par_unseq: " << avg_abs_diff_3D << " spiral GIPR: " << s3D_gipr << " classic par_unseq GIPR: " << cpu3D_gipr);
		BOOST_TEST_MESSAGE("3D spiral avg GI : " << s3D_avg_gi << " classic par_unseq avg GI : " << cpu3D_avg_gi);
		BOOST_TEST_MESSAGE("3D spiral output histogram:");
		make_and_print_histogram<20>(yagit::core::data::make_const_view(out3D), epsilon);
		BOOST_TEST_MESSAGE("3D classic par_unseq output histogram:");
		make_and_print_histogram<20>(yagit::core::data::make_const_view(data3D.output_par_unseq), epsilon);
	};
	auto make_comparison_global = [&]() 
	{
		BOOST_TEST_MESSAGE("\t\t1D");
		auto classic_vectorized_timings_1D = run_vectorized_gi_on_data(data1D, global_params);
		BOOST_TEST_MESSAGE("\t\t\tTook: " << classic_vectorized_timings_1D[3] << 's');
		BOOST_TEST_MESSAGE("\t\t2D");
		auto classic_vectorized_timings_2D = run_vectorized_gi_on_data(data2D, global_params);
		BOOST_TEST_MESSAGE("\t\t\tTook: " << classic_vectorized_timings_2D[3] << 's');
		BOOST_TEST_MESSAGE("\t\t3D");
		auto classic_vectorized_timings_3D = run_vectorized_gi_on_data(data3D, global_params);
		BOOST_TEST_MESSAGE("\t\t\tTook: " << classic_vectorized_timings_3D[3] << 's');

		std::vector<double> out1D(total_size(data1D.ref_size));
		std::vector<double> out2D(total_size(data2D.ref_size));
		std::vector<double> out3D(total_size(data3D.ref_size));

		auto spiral_timings123D = run_spiral_tests(
			data1D, data2D, data3D,
			yagit::core::data::view<double>{std::data(out1D), std::data(out1D) + std::size(out1D)},
			yagit::core::data::view<double>{std::data(out2D), std::data(out2D) + std::size(out2D)},
			yagit::core::data::view<double>{std::data(out3D), std::data(out3D) + std::size(out3D)},
			[](auto&& ref1D, auto&& tar1D)
			{
				SpiralRectangleSolver1D solver(ref1D, tar1D, 1.0, global_params.distance_to_agreement_normalization());
				solver.setLocal(false);
				solver.setReferenceValue(global_params.absolute_dose_difference());
				return solver.calculateGamma();
			},
			[](auto&& ref2D, auto&& tar2D)
			{
				SpiralRectangleSolver2D solver(ref2D, tar2D, 1.0, global_params.distance_to_agreement_normalization());
				solver.setLocal(false);
				solver.setReferenceValue(global_params.absolute_dose_difference());
				return solver.calculateGamma();
			},
				[](auto&& ref3D, auto&& tar3D)
			{
				SpiralRectangleSolver3D solver(ref3D, tar3D, 1.0, global_params.distance_to_agreement_normalization());
				solver.setLocal(false);
				solver.setReferenceValue(global_params.absolute_dose_difference());
				return solver.calculateGamma();
			}
			);

		// do compare
		auto avg_abs_diff_1D = yagit::additions::avg_abs_gi_difference(
			yagit::core::data::make_const_view(data1D.output_par_unseq),
			yagit::core::data::make_const_view(out1D)
		);
		auto avg_abs_diff_2D = yagit::additions::avg_abs_gi_difference(
			yagit::core::data::make_const_view(data2D.output_par_unseq),
			yagit::core::data::make_const_view(out2D)
		);
		auto avg_abs_diff_3D = yagit::additions::avg_abs_gi_difference(
			yagit::core::data::make_const_view(data3D.output_par_unseq),
			yagit::core::data::make_const_view(out3D)
		);

		auto s1D_gipr = yagit::additions::active_voxels_passing_rate(
			yagit::core::data::make_const_view(out1D)
		);
		auto s2D_gipr = yagit::additions::active_voxels_passing_rate(
			yagit::core::data::make_const_view(out2D)
		);
		auto s3D_gipr = yagit::additions::active_voxels_passing_rate(
			yagit::core::data::make_const_view(out3D)
		);

		auto s1D_avg_gi = yagit::additions::avg(
			yagit::core::data::make_const_view(out1D)
		);
		auto s2D_avg_gi = yagit::additions::avg(
			yagit::core::data::make_const_view(out2D)
		);
		auto s3D_avg_gi = yagit::additions::avg(
			yagit::core::data::make_const_view(out3D)
		);

		auto cpu1D_gipr = yagit::additions::active_voxels_passing_rate(
			yagit::core::data::make_const_view(data1D.output_par_unseq)
		);
		auto cpu2D_gipr = yagit::additions::active_voxels_passing_rate(
			yagit::core::data::make_const_view(data2D.output_par_unseq)
		);
		auto cpu3D_gipr = yagit::additions::active_voxels_passing_rate(
			yagit::core::data::make_const_view(data3D.output_par_unseq)
		);

		auto cpu1D_avg_gi = yagit::additions::avg(
			yagit::core::data::make_const_view(data1D.output_par_unseq)
		);
		auto cpu2D_avg_gi = yagit::additions::avg(
			yagit::core::data::make_const_view(data2D.output_par_unseq)
		);
		auto cpu3D_avg_gi = yagit::additions::avg(
			yagit::core::data::make_const_view(data3D.output_par_unseq)
		);

		BOOST_TEST_MESSAGE("1D spiral vs par_unseq: " << avg_abs_diff_1D << " spiral GIPR: " << s1D_gipr << " classic par_unseq GIPR: " << cpu1D_gipr);
		BOOST_TEST_MESSAGE("1D spiral avg GI : " << s1D_avg_gi << " classic par_unseq avg GI : " << cpu1D_avg_gi);
		BOOST_TEST_MESSAGE("1D spiral output histogram:");
		make_and_print_histogram<20>(yagit::core::data::make_const_view(out1D), epsilon);
		BOOST_TEST_MESSAGE("1D classic par_unseq output histogram:");
		make_and_print_histogram<20>(yagit::core::data::make_const_view(data1D.output_par_unseq), epsilon);
		BOOST_TEST_MESSAGE("2D spiral vs par_unseq: " << avg_abs_diff_2D << " spiral GIPR: " << s2D_gipr << " classic par_unseq GIPR: " << cpu2D_gipr);
		BOOST_TEST_MESSAGE("2D spiral avg GI : " << s2D_avg_gi << " classic par_unseq avg GI : " << cpu2D_avg_gi);
		BOOST_TEST_MESSAGE("2D spiral output histogram:");
		make_and_print_histogram<20>(yagit::core::data::make_const_view(out2D), epsilon);
		BOOST_TEST_MESSAGE("2D classic par_unseq output histogram:");
		make_and_print_histogram<20>(yagit::core::data::make_const_view(data2D.output_par_unseq), epsilon);
		BOOST_TEST_MESSAGE("3D spiral vs par_unseq: " << avg_abs_diff_3D << " spiral GIPR: " << s3D_gipr << " classic par_unseq GIPR: " << cpu3D_gipr);
		BOOST_TEST_MESSAGE("3D spiral avg GI : " << s3D_avg_gi << " classic par_unseq avg GI : " << cpu3D_avg_gi);
		BOOST_TEST_MESSAGE("3D spiral output histogram:");
		make_and_print_histogram<20>(yagit::core::data::make_const_view(out3D), epsilon);
		BOOST_TEST_MESSAGE("3D classic par_unseq output histogram:");
		make_and_print_histogram<20>(yagit::core::data::make_const_view(data3D.output_par_unseq), epsilon);
	};

	BOOST_TEST_MESSAGE("Unfiltered local");
	make_comparison_local();
	BOOST_TEST_MESSAGE("Unfiltered global");
	make_comparison_global();

	yagit::additions::filter_image_data_based_on_dose_cutoff(data1D.doses_ref(), dose_cutoff);
	yagit::additions::filter_image_data_based_on_dose_cutoff(data1D.doses_tar(), dose_cutoff);
	yagit::additions::filter_image_data_based_on_dose_cutoff(data2D.doses_ref(), dose_cutoff);
	yagit::additions::filter_image_data_based_on_dose_cutoff(data2D.doses_tar(), dose_cutoff);
	yagit::additions::filter_image_data_based_on_dose_cutoff(data3D.doses_ref(), dose_cutoff);
	yagit::additions::filter_image_data_based_on_dose_cutoff(data3D.doses_tar(), dose_cutoff);

	BOOST_TEST_MESSAGE("Filtered local");
	make_comparison_local();
	BOOST_TEST_MESSAGE("Filtered global");
	make_comparison_global();
}