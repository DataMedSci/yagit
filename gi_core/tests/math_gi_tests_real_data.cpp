#include <math/gamma_index.hpp>
#include <math/vectorized/gamma_index_single.hpp>
#include <boost/test/unit_test.hpp>
#include <solver.h>
#include <image.h>

#include <chrono>
#include <fstream>
#include <variant>
#include <filesystem>

using namespace yagit::core::math;
using namespace yagit::core::data;
using namespace std;
using namespace std::chrono;

template<typename ElementType>
struct dataset_static_info
{
	sizes<3> image_size;
	array<ElementType, 3> image_position;
	array<ElementType, 3> image_spacing;
};

template<typename ElementType>
struct raw_dataset
{
	dataset_static_info<ElementType> info;
	unique_ptr<ElementType[]> image_data;
};

using raw_dataset_t = optional<variant<raw_dataset<float>, raw_dataset<double>>>;

template<typename ElementType>
raw_dataset_t load_raw_dataset_typed(const std::byte* input, size_t filesize)
{
	size_t required_filesize = sizeof(uint64_t) + sizeof(dataset_static_info<ElementType>);
	if (filesize < required_filesize)
		return nullopt;

	raw_dataset<ElementType> dataset;

	std::memcpy(&dataset.info.image_size, input, sizeof(dataset.info.image_size));
	input += sizeof(dataset.info.image_size);

	auto total_image_element_count = total_size(dataset.info.image_size);
	auto total_image_size_in_bytes = total_image_element_count * sizeof(ElementType);
	required_filesize += total_image_size_in_bytes;
	if (filesize < required_filesize)
		return nullopt;

	std::memcpy(&dataset.info.image_position, input, sizeof(dataset.info.image_position));
	input += sizeof(dataset.info.image_position);
	std::memcpy(&dataset.info.image_spacing, input, sizeof(dataset.info.image_spacing));
	input += sizeof(dataset.info.image_spacing);

	dataset.image_data = make_unique<ElementType[]>(total_image_element_count);

	std::memcpy(dataset.image_data.get(), input, total_image_size_in_bytes);

	return dataset;
}

raw_dataset_t load_raw_dataset(const filesystem::path& path)
{
	vector<std::byte> data;
	size_t filesize = 0;

	{
		ifstream input(path, ifstream::binary);
		if (!input.is_open())
			return nullopt;

		input.seekg(0, ios::end);
		filesize = input.tellg();
		input.seekg(0, ios::beg);

		data.resize(filesize);

		input.read(reinterpret_cast<char*>(data.data()), filesize);
	}
	
	const auto* cur_data = data.data();

	size_t required_filesize = sizeof(uint64_t);
	if (filesize < required_filesize)
		return nullopt;

	uint64_t is_float32;
	std::memcpy(&is_float32, cur_data, sizeof(is_float32));
	cur_data += sizeof(is_float32);

	if (is_float32)
		return load_raw_dataset_typed<float>(cur_data, filesize);
	else
		return load_raw_dataset_typed<double>(cur_data, filesize);
}

template<typename ReferenceType, typename TargetType>
raw_dataset_t force_convert(const raw_dataset<ReferenceType>& ref, const raw_dataset<TargetType>& tar)
{
	raw_dataset<ReferenceType> converted_target_type;
	converted_target_type.info.image_size = tar.info.image_size;
	converted_target_type.info.image_position = {
		static_cast<ReferenceType>(tar.info.image_position[0]),
		static_cast<ReferenceType>(tar.info.image_position[1]),
		static_cast<ReferenceType>(tar.info.image_position[2])
	};
	converted_target_type.info.image_spacing = {
		static_cast<ReferenceType>(tar.info.image_spacing[0]),
		static_cast<ReferenceType>(tar.info.image_spacing[1]),
		static_cast<ReferenceType>(tar.info.image_spacing[2])
	};

	auto total_image_element_count = total_size(tar.info.image_size);
	converted_target_type.image_data = make_unique<ReferenceType[]>(total_image_element_count);
	transform(tar.image_data.get(), tar.image_data.get() + total_image_element_count, converted_target_type.image_data.get(), [](auto&& v) {return static_cast<ReferenceType>(v); });

	return converted_target_type;
}

template<typename ElementType>
struct prepared_dataset
{
	using type = ElementType;
	dataset_static_info<ElementType> info;
	unique_ptr<ElementType[]> image_datapoints;
	array<unique_ptr<ElementType[]>, 3> image_coords;
};

using prepared_dataset_t = variant<prepared_dataset<float>, prepared_dataset<double>>;

template<typename ElementType>
prepared_dataset<ElementType> prepare_dataset(raw_dataset<ElementType>&& raw_ds)
{
	prepared_dataset<ElementType> p_ds;
	p_ds.info = raw_ds.info;
	p_ds.image_datapoints = move(raw_ds.image_data);
	auto total_image_element_count = total_size(raw_ds.info.image_size);
	p_ds.image_coords[0] = make_unique<ElementType[]>(total_image_element_count);
	p_ds.image_coords[1] = make_unique<ElementType[]>(total_image_element_count);
	p_ds.image_coords[2] = make_unique<ElementType[]>(total_image_element_count);

	auto x_c = p_ds.image_coords[0].get();
	auto y_c = p_ds.image_coords[1].get();
	auto z_c = p_ds.image_coords[2].get();

	auto y_skip = raw_ds.info.image_size.sizes[2];
	auto x_skip = raw_ds.info.image_size.sizes[2] * raw_ds.info.image_size.sizes[1];

	for (size_t x = 0; x < raw_ds.info.image_size.sizes[0]; ++x)
	{
		auto x_c_end = x_c + x_skip;
		fill(x_c, x_c_end, raw_ds.info.image_position[0] + raw_ds.info.image_spacing[0] * x);
		x_c = x_c_end;

		for (size_t y = 0; y < raw_ds.info.image_size.sizes[1]; ++y)
		{
			auto y_c_end = y_c + y_skip;
			fill(y_c, y_c_end, raw_ds.info.image_position[1] + raw_ds.info.image_spacing[1] * y);
			y_c = y_c_end;

			for (size_t z = 0; z < raw_ds.info.image_size.sizes[2]; ++z)
			{
				*z_c++ = raw_ds.info.image_position[2] + raw_ds.info.image_spacing[2] * z;
			}
		}
	}

	return p_ds;
}

template<typename ElementType, template<typename> typename GIParams>
struct dataset_results
{
	dataset_static_info<ElementType> ref_info;
	dataset_static_info<ElementType> tar_info;
	GIParams<ElementType> params;
	unique_ptr<ElementType[]> gi_output;

	nanoseconds time_taken;
};

template<template<typename> typename GIParams, typename ElementType>
optional<dataset_results<ElementType, GIParams>> run_gi(
	const prepared_dataset<ElementType>& reference, const prepared_dataset<ElementType>& target,
	ElementType distance_to_agreement,
	ElementType percentage_or_dose_difference)
{
	dataset_results<ElementType, GIParams> results;

	results.ref_info = reference.info;
	results.tar_info = target.info;

	results.params.distance_to_agreement_squared = distance_to_agreement * distance_to_agreement;
	if constexpr (is_same_v<GIParams<ElementType>, local_gamma_index_params<ElementType>>)
		results.params.percentage = percentage_or_dose_difference;
	else if constexpr (is_same_v<GIParams<ElementType>, global_gamma_index_params<ElementType>>)
		results.params.dose_difference_squared = percentage_or_dose_difference * percentage_or_dose_difference;

	auto total_image_element_count = total_size(reference.info.image_size);
	results.gi_output = make_unique<ElementType[]>(total_image_element_count);

	auto start = high_resolution_clock::now();
	parallel_unsequenced_gamma_index_implementer<ElementType, 3>::initialize_pass(results.gi_output.get(), results.gi_output.get() + total_image_element_count);
	parallel_unsequenced_gamma_index_implementer<ElementType, 3>::minimize_pass(
		results.gi_output.get(), results.gi_output.get() + total_image_element_count,
		reference.image_datapoints.get(),
		array<const ElementType*, 3>{reference.image_coords[0].get(), reference.image_coords[1].get(), reference.image_coords[2].get()},
		target.image_datapoints.get(), target.image_datapoints.get() + total_image_element_count,
		array<const ElementType*, 3>{target.image_coords[0].get(), target.image_coords[1].get(), target.image_coords[2].get()},
		results.params
	);
	parallel_unsequenced_gamma_index_implementer<ElementType, 3>::finalize_pass(results.gi_output.get(), results.gi_output.get() + total_image_element_count);
	auto end = high_resolution_clock::now();

	results.time_taken = duration_cast<nanoseconds>(end - start);

	return results;
}

template<typename ElementType>
Image3D make_image(const prepared_dataset<ElementType>& dataset)
{
	vector<vector<vector<double>>> data(
		dataset.info.image_size.sizes[0],
		vector<vector<double>>(
			dataset.info.image_size.sizes[1],
			vector<double>(dataset.info.image_size.sizes[2], double())
			));

	for (size_t x = 0; x < dataset.info.image_size.sizes[0]; x++)
		for (size_t y = 0; y < dataset.info.image_size.sizes[1]; y++)
			for (size_t z = 0; z < dataset.info.image_size.sizes[2]; z++)
				data[x][y][z] = dataset.image_datapoints[(x * dataset.info.image_size.sizes[1] + y) * dataset.info.image_size.sizes[2] + z];

	
	return Image3D(
		{ dataset.info.image_position[0], dataset.info.image_position[1], dataset.info.image_position[2] },
		{ dataset.info.image_spacing[0], dataset.info.image_spacing[1], dataset.info.image_spacing[2] },
		data
	);
}

template<typename ElementType>
unique_ptr<ElementType[]> convert_to_data(const Image3D& img)
{
	unique_ptr<ElementType[]> data = make_unique<ElementType[]>(img.getNelems());

	auto shape = img.getShape();
	for (size_t x = 0; x < shape[0]; x++)
		for (size_t y = 0; y < shape[1]; y++)
			for (size_t z = 0; z < shape[2]; z++)
				data[(x * shape[1] + y) * shape[2] + z] = img.data()[x][y][z];

	return data;
}


template<template<typename> typename GIParams, typename ElementType>
optional<dataset_results<ElementType, GIParams>> run_gi_spiral_rectangle(
	const prepared_dataset<ElementType>& reference, const prepared_dataset<ElementType>& target,
	ElementType distance_to_agreement,
	ElementType percentage_or_dose_difference)
{
	dataset_results<ElementType, GIParams> results;

	auto ref_image = make_image(reference);
	auto tar_image = make_image(target);

	SpiralRectangleSolver3D solver(ref_image, tar_image, percentage_or_dose_difference, distance_to_agreement);

	results.params.distance_to_agreement_squared = distance_to_agreement * distance_to_agreement;
	if constexpr (is_same_v<GIParams<ElementType>, local_gamma_index_params<ElementType>>)
	{
		solver.setLocal(true);
		results.params.percentage = percentage_or_dose_difference;
	}
	else if constexpr (is_same_v<GIParams<ElementType>, global_gamma_index_params<ElementType>>)
	{
		solver.setLocal(false);
		solver.calculateDD();
		results.params.dose_difference_squared = percentage_or_dose_difference * percentage_or_dose_difference;
	}

	results.ref_info = reference.info;
	results.tar_info = target.info;

	auto start = high_resolution_clock::now();
	auto output = solver.calculateGamma();
	auto end = high_resolution_clock::now();
	results.gi_output = convert_to_data<ElementType>(output);
	results.time_taken = duration_cast<nanoseconds>(end - start);

	return results;
}


template<typename ElementType>
ElementType get_distance_to_agreement(const prepared_dataset<ElementType>& reference)
{
	return std::max({ reference.info.image_spacing[0], reference.info.image_spacing[1], reference.info.image_spacing[2] }) * 10;
}

template<typename ElementType>
ElementType find_normalization_dose(const prepared_dataset<ElementType>& reference)
{
	auto total_image_element_count = total_size(reference.info.image_size);
	auto el_it = std::max_element(reference.image_datapoints.get(), reference.image_datapoints.get() + total_image_element_count, [](auto&& largest, auto&& el) {return abs(largest) < abs(el); });
	return *el_it;
}

BOOST_AUTO_TEST_CASE(real_data_old_spiral_with_rectangle)
{
	using ref_tar_t = pair<string_view, string_view>;
	array image_pairs =
	{
		ref_tar_t{"data/raw/Ref.raw", "data/raw/Eval.raw"},
		ref_tar_t{"data/raw/m1.raw", "data/raw/m2.raw"},
		ref_tar_t{"data/raw/m3.raw", "data/raw/m4.raw"},
		//ref_tar_t{"data/raw/Ref_highRes.raw", "data/raw/Eval_highRes.raw"}
	};

	BOOST_TEST_MESSAGE("Spiral solver");

	for (auto& ref_tar : image_pairs)
	{
		BOOST_TEST_MESSAGE("Reference: " << ref_tar.first);
		BOOST_TEST_MESSAGE("Target: " << ref_tar.second);

		auto raw_reference = load_raw_dataset(ref_tar.first);
		auto raw_target = load_raw_dataset(ref_tar.second);

		BOOST_ASSERT(raw_reference && raw_target);

		auto conv_raw_target = std::visit([](auto&& raw_rds, auto&& raw_tds) {return force_convert(raw_rds, raw_tds); }, *raw_reference, *raw_target);

		auto prepared_reference = std::visit([](auto&& raw_ds)->prepared_dataset_t { return prepare_dataset(std::move(raw_ds)); }, std::move(*raw_reference));
		auto prepared_target = std::visit([](auto&& raw_ds)->prepared_dataset_t { return prepare_dataset(std::move(raw_ds)); }, std::move(*conv_raw_target));

		auto distance_to_agreement = std::visit([](auto&& p_ds)->variant<float, double> { return get_distance_to_agreement(p_ds); }, prepared_reference);
		auto normalization_dose = std::visit([](auto&& p_ds)->variant<float, double> { return find_normalization_dose(p_ds); }, prepared_reference);
		auto percentage_f = 0.02f;
		auto percentage_d = 0.02;

		std::visit([&](auto&& v) { BOOST_TEST_MESSAGE("Global normalization dose: " << v); }, normalization_dose);
		BOOST_TEST_MESSAGE("Percentage: " << percentage_f);

		if (prepared_reference.index() == 0)
		{
			auto results_local = run_gi_spiral_rectangle<local_gamma_index_params>(get<0>(prepared_reference), get<0>(prepared_target), get<0>(distance_to_agreement), percentage_f);
			if (results_local)
				BOOST_TEST_MESSAGE("Time taken local: " << results_local->time_taken.count() / 1e9 << "s");
			auto results_global = run_gi_spiral_rectangle<global_gamma_index_params>(get<0>(prepared_reference), get<0>(prepared_target), get<0>(distance_to_agreement), get<0>(normalization_dose));
			if (results_local)
				BOOST_TEST_MESSAGE("Time taken global: " << results_global->time_taken.count() / 1e9 << "s");
		}
		else
		{
			auto results_local = run_gi_spiral_rectangle<local_gamma_index_params>(get<1>(prepared_reference), get<1>(prepared_target), get<1>(distance_to_agreement), percentage_d);
			if (results_local)
				BOOST_TEST_MESSAGE("Time taken local: " << results_local->time_taken.count() / 1e9 << "s");
			auto results_global = run_gi_spiral_rectangle<global_gamma_index_params>(get<1>(prepared_reference), get<1>(prepared_target), get<1>(distance_to_agreement), get<1>(normalization_dose));
			if (results_local)
				BOOST_TEST_MESSAGE("Time taken global: " << results_global->time_taken.count() / 1e9 << "s");
		}
	}
}

BOOST_AUTO_TEST_CASE(real_data_new_method)
{
	using ref_tar_t = pair<string_view, string_view>;
	array image_pairs =
	{
		ref_tar_t{"data/raw/Ref.raw", "data/raw/Eval.raw"},
		ref_tar_t{"data/raw/m1.raw", "data/raw/m2.raw"},
		ref_tar_t{"data/raw/m3.raw", "data/raw/m4.raw"},
		//ref_tar_t{"data/raw/Ref_highRes.raw", "data/raw/Eval_highRes.raw"}
	};
	
	BOOST_TEST_MESSAGE("Vectorized MT");

	for (auto& ref_tar : image_pairs)
	{
		BOOST_TEST_MESSAGE("Reference: " << ref_tar.first);
		BOOST_TEST_MESSAGE("Target: " << ref_tar.second);

		auto raw_reference = load_raw_dataset(ref_tar.first);
		auto raw_target = load_raw_dataset(ref_tar.second);

		BOOST_ASSERT(raw_reference && raw_target);

		auto conv_raw_target = std::visit([](auto&& raw_rds, auto&& raw_tds) {return force_convert(raw_rds, raw_tds); }, *raw_reference, *raw_target);

		auto prepared_reference = std::visit([](auto&& raw_ds)->prepared_dataset_t { return prepare_dataset(std::move(raw_ds)); }, std::move(*raw_reference));
		auto prepared_target = std::visit([](auto&& raw_ds)->prepared_dataset_t { return prepare_dataset(std::move(raw_ds)); }, std::move(*conv_raw_target));

		auto distance_to_agreement = std::visit([](auto&& p_ds)->variant<float, double> { return get_distance_to_agreement(p_ds); }, prepared_reference);
		auto normalization_dose = std::visit([](auto&& p_ds)->variant<float, double> { return find_normalization_dose(p_ds); }, prepared_reference);
		auto percentage_f = 0.02f;
		auto percentage_d = 0.02;

		std::visit([&](auto&& v) { BOOST_TEST_MESSAGE("Global normalization dose: " << v); }, normalization_dose);
		BOOST_TEST_MESSAGE("Percentage: " << percentage_f);

		if (prepared_reference.index() == 0)
		{
			auto results_local = run_gi<local_gamma_index_params>(get<0>(prepared_reference), get<0>(prepared_target), get<0>(distance_to_agreement), percentage_f);
			if (results_local)
				BOOST_TEST_MESSAGE("Time taken local: " << results_local->time_taken.count() / 1e9 << "s");
			auto results_global = run_gi<global_gamma_index_params>(get<0>(prepared_reference), get<0>(prepared_target), get<0>(distance_to_agreement), get<0>(normalization_dose));
			if (results_local)
				BOOST_TEST_MESSAGE("Time taken global: " << results_global->time_taken.count() / 1e9 << "s");
		}
		else
		{
			auto results_local = run_gi<local_gamma_index_params>(get<1>(prepared_reference), get<1>(prepared_target), get<1>(distance_to_agreement), percentage_d);
			if(results_local)
				BOOST_TEST_MESSAGE("Time taken local: " << results_local->time_taken.count()/1e9 << "s");
			auto results_global = run_gi<global_gamma_index_params>(get<1>(prepared_reference), get<1>(prepared_target), get<1>(distance_to_agreement), get<1>(normalization_dose));
			if (results_local)
				BOOST_TEST_MESSAGE("Time taken global: " << results_global->time_taken.count() / 1e9 << "s");
		}
	}
}