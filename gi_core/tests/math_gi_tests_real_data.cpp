/*#include <math/gamma_index.hpp>
#include <boost/test/unit_test.hpp>
#include <solver.h>
#include <image.h>
#include "../../gi_additions/include/additions.hpp"

#include <chrono>
#include <fstream>
#include <variant>
#include <filesystem>

using namespace yagit::core::math;
using namespace yagit::core::data;
using namespace std;
using namespace std::chrono;
using namespace yagit::core::math::execution;

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

	auto inverted_image_data = make_unique<ElementType[]>(total_image_element_count);
	dataset.image_data = make_unique<ElementType[]>(total_image_element_count);

	std::memcpy(inverted_image_data.get(), input, total_image_size_in_bytes);

	for (size_t x = 0; x < dataset.info.image_size.sizes[0]; ++x)
	{
		for (size_t y = 0; y < dataset.info.image_size.sizes[1]; ++y)
		{
			for (size_t z = 0; z < dataset.info.image_size.sizes[2]; ++z)
			{
				dataset.image_data[(z * dataset.info.image_size.sizes[1] + y) * dataset.info.image_size.sizes[0] + x]
					= inverted_image_data[(x * dataset.info.image_size.sizes[1] + y) * dataset.info.image_size.sizes[2] + z];
			}
		}
	}

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

	auto y_skip = raw_ds.info.image_size.sizes[0];
	auto x_skip = raw_ds.info.image_size.sizes[0] * raw_ds.info.image_size.sizes[1];

	for (size_t x = 0; x < raw_ds.info.image_size.sizes[2]; ++x)
	{
		auto x_c_end = x_c + x_skip;
		fill(x_c, x_c_end, raw_ds.info.image_position[2] + raw_ds.info.image_spacing[2] * x);
		x_c = x_c_end;

		for (size_t y = 0; y < raw_ds.info.image_size.sizes[1]; ++y)
		{
			auto y_c_end = y_c + y_skip;
			fill(y_c, y_c_end, raw_ds.info.image_position[1] + raw_ds.info.image_spacing[1] * y);
			y_c = y_c_end;

			for (size_t z = 0; z < raw_ds.info.image_size.sizes[0]; ++z)
			{
				*z_c++ = raw_ds.info.image_position[0] + raw_ds.info.image_spacing[0] * z;
			}
		}
	}

	{
		auto count_not_nan = yagit::additions::active_voxels_count(p_ds.image_datapoints.get(), p_ds.image_datapoints.get() + total_image_element_count);
		BOOST_TEST_MESSAGE("Input dataset voxel count not nan: " << count_not_nan);
		BOOST_TEST_MESSAGE("Input dataset voxel count total: " << total_image_element_count);
		BOOST_TEST_MESSAGE("Input dataset histogram (pre-filtered): ");
		constexpr size_t bins_count = 10;
		auto our_min = yagit::additions::min(p_ds.image_datapoints.get(), p_ds.image_datapoints.get() + total_image_element_count);
		auto our_max = yagit::additions::max(p_ds.image_datapoints.get(), p_ds.image_datapoints.get() + total_image_element_count);
		auto our_bin_spacing = (our_max - our_min) / bins_count;
		array<ElementType, bins_count + 1> our_dels;
		array<size_t, bins_count + 2> our_bins;
		our_dels.front() = our_min;
		our_dels.back() = our_max;
		for (size_t i = 1; i < bins_count; i++)
		{
			our_dels[i] = our_min + i * our_bin_spacing;
		}
		yagit::additions::make_histogram<true>(our_bins.data(), p_ds.image_datapoints.get(), p_ds.image_datapoints.get() + total_image_element_count, our_dels.data(), our_dels.data() + our_dels.size());
		for (size_t i = 0; i < bins_count; i++)
		{
			BOOST_TEST_MESSAGE("Range<" << our_dels[i] << ", " << our_dels[i + 1] << ">: " << our_bins[i]);
		}
		BOOST_TEST_MESSAGE("Not in delimeters: " << our_bins[bins_count]);
		BOOST_TEST_MESSAGE("NaNs: " << our_bins[bins_count + 1]);
	}

	yagit::additions::filter_image_data_based_on_dose_cutoff(p_ds.image_datapoints.get(), p_ds.image_datapoints.get() + total_image_element_count, static_cast<ElementType>(0.05));

	{
		auto count_not_nan = yagit::additions::active_voxels_count(p_ds.image_datapoints.get(), p_ds.image_datapoints.get() + total_image_element_count);
		BOOST_TEST_MESSAGE("Input dataset voxel count not nan: " << count_not_nan);
		BOOST_TEST_MESSAGE("Input dataset voxel count total: " << total_image_element_count);
		BOOST_TEST_MESSAGE("Input dataset histogram (post-filtered): ");
		constexpr size_t bins_count = 10;
		auto our_min = yagit::additions::min(p_ds.image_datapoints.get(), p_ds.image_datapoints.get() + total_image_element_count);
		auto our_max = yagit::additions::max(p_ds.image_datapoints.get(), p_ds.image_datapoints.get() + total_image_element_count);
		auto our_bin_spacing = (our_max - our_min) / bins_count;
		array<ElementType, bins_count + 1> our_dels;
		array<size_t, bins_count + 2> our_bins;
		our_dels.front() = our_min;
		our_dels.back() = our_max;
		for (size_t i = 1; i < bins_count; i++)
		{
			our_dels[i] = our_min + i * our_bin_spacing;
		}
		yagit::additions::make_histogram<true>(our_bins.data(), p_ds.image_datapoints.get(), p_ds.image_datapoints.get() + total_image_element_count, our_dels.data(), our_dels.data() + our_dels.size());
		for (size_t i = 0; i < bins_count; i++)
		{
			BOOST_TEST_MESSAGE("Range<" << our_dels[i] << ", " << our_dels[i + 1] << ">: " << our_bins[i]);
		}
		BOOST_TEST_MESSAGE("Not in delimeters: " << our_bins[bins_count]);
		BOOST_TEST_MESSAGE("NaNs: " << our_bins[bins_count + 1]);
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
	gamma_index(algorithm_version::classic{}, parallel_policy{}, results.gi_output.get(), results.gi_output.get() + total_image_element_count,
		reference.image_datapoints.get(),
		array<const ElementType*, 3>{reference.image_coords[0].get(), reference.image_coords[1].get(), reference.image_coords[2].get()},
		target.image_datapoints.get(), target.image_datapoints.get() + total_image_element_count,
		array<const ElementType*, 3>{target.image_coords[0].get(), target.image_coords[1].get(), target.image_coords[2].get()},
		results.params);
	auto end = high_resolution_clock::now();

	results.time_taken = duration_cast<nanoseconds>(end - start);

	return results;
}

template<typename ElementType>
Image3D make_image(const prepared_dataset<ElementType>& dataset)
{
	vector<vector<vector<double>>> data(
		dataset.info.image_size.sizes[2],
		vector<vector<double>>(
			dataset.info.image_size.sizes[1],
			vector<double>(dataset.info.image_size.sizes[0], double())
			));

	for (size_t x = 0; x < dataset.info.image_size.sizes[0]; x++)
		for (size_t y = 0; y < dataset.info.image_size.sizes[1]; y++)
			for (size_t z = 0; z < dataset.info.image_size.sizes[2]; z++)
				data[z][y][x] = dataset.image_datapoints[(x * dataset.info.image_size.sizes[1] + y) * dataset.info.image_size.sizes[2] + z];

	
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
struct raw_output_data
{
	sizes<3> image_size;
	unique_ptr<ElementType[]> image_data;
};

using raw_output_data_t = optional<variant<raw_output_data<float>, raw_output_data<double>>>;

template<typename ElementType>
raw_output_data_t load_raw_output_typed(const std::byte* input, size_t filesize)
{
	size_t required_filesize = sizeof(uint64_t) + sizeof(sizes<3>);
	if (filesize < required_filesize)
		return nullopt;

	raw_output_data<ElementType> dataset;

	std::memcpy(&dataset.image_size, input, sizeof(dataset.image_size));
	input += sizeof(dataset.image_size);

	auto total_image_element_count = total_size(dataset.image_size);
	auto total_image_size_in_bytes = total_image_element_count * sizeof(ElementType);
	required_filesize += total_image_size_in_bytes;
	if (filesize < required_filesize)
		return nullopt;

	dataset.image_data = make_unique<ElementType[]>(total_image_element_count);

	std::memcpy(dataset.image_data.get(), input, total_image_size_in_bytes);

	return dataset;
}

raw_output_data_t load_raw_output(const filesystem::path& path)
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
		return load_raw_output_typed<float>(cur_data, filesize);
	else
		return load_raw_output_typed<double>(cur_data, filesize);
}

template<typename ElementType>
ElementType get_distance_to_agreement(const prepared_dataset<ElementType>& reference)
{
	return static_cast<ElementType>(2.0);
}

template<typename ElementType>
ElementType find_normalization_dose(const prepared_dataset<ElementType>& reference)
{
	auto total_image_element_count = total_size(reference.info.image_size);
	return yagit::additions::max(reference.image_datapoints.get(), reference.image_datapoints.get() + total_image_element_count);
}

template<template<typename> typename GIParams, typename ElementType>
bool save_results(const filesystem::path& path, const dataset_results<ElementType, GIParams>& data)
{
	ofstream output(path, ofstream::binary);
	if (!output.is_open())
		return false;

	uint64_t isfloat32 = is_same_v<ElementType, float>;
	output.write(reinterpret_cast<const char*>(&isfloat32), sizeof(isfloat32));

	output.write(reinterpret_cast<const char*>(&data.ref_info), sizeof(data.ref_info));
	output.write(reinterpret_cast<const char*>(&data.tar_info), sizeof(data.tar_info));
	output.write(reinterpret_cast<const char*>(&data.params), sizeof(data.params));
	int64_t nanoseconds = data.time_taken.count();
	output.write(reinterpret_cast<const char*>(&nanoseconds), sizeof(nanoseconds));
	output.write(reinterpret_cast<const char*>(data.gi_output.get()), total_size(data.ref_info.image_size));

	return true;
}

template<template<typename> typename GIParams, typename ElementType, typename ElementTypeO>
void compare_results(const dataset_results<ElementType, GIParams>& data, const raw_output_data<ElementTypeO>& other_data)
{
	auto size_our = total_size(data.ref_info.image_size);
	auto size_their = total_size(other_data.image_size);
	BOOST_ASSERT(size_our == size_their);
	
	yagit::additions::filter_image_data_based_on_dose_cutoff(other_data.image_data.get(), other_data.image_data.get() + size_their, static_cast<ElementTypeO>(0));

	auto gi_pr_our = yagit::additions::active_voxels_passing_rate(data.gi_output.get(), data.gi_output.get() + size_our);
	auto gi_pr_their = yagit::additions::active_voxels_passing_rate(other_data.image_data.get(), other_data.image_data.get() + size_their);
	BOOST_TEST_MESSAGE("GI Passing Rate (Our): " << gi_pr_our);
	BOOST_TEST_MESSAGE("GI Passing Rate (Their): " << gi_pr_their);

	constexpr size_t bins_count = 10;
	auto our_min = yagit::additions::min(data.gi_output.get(), data.gi_output.get() + size_our);
	auto their_min = yagit::additions::min(other_data.image_data.get(), other_data.image_data.get() + size_their);
	auto our_max = yagit::additions::max(data.gi_output.get(), data.gi_output.get() + size_our);
	auto their_max = yagit::additions::max(other_data.image_data.get(), other_data.image_data.get() + size_their);
	auto our_bin_spacing = (our_max - our_min) / bins_count;
	auto their_bin_spacing = (their_max - their_min) / bins_count;
	array<ElementType, bins_count + 1> our_dels;
	array<size_t, bins_count + 2> our_bins;
	our_dels.front() = our_min;
	our_dels.back() = our_max;
	array<ElementTypeO, bins_count + 1> their_dels;
	array<size_t, bins_count + 2> their_bins;
	their_dels.front() = their_min;
	their_dels.back() = their_max;
	for (size_t i = 1; i < bins_count; i++)
	{
		our_dels[i] = our_min + i * our_bin_spacing;
		their_dels[i] = their_min + i * their_bin_spacing;
	}
	yagit::additions::make_histogram<true>(our_bins.data(), data.gi_output.get(), data.gi_output.get() + size_our, our_dels.data(), our_dels.data() + our_dels.size());
	yagit::additions::make_histogram<true>(their_bins.data(), other_data.image_data.get(), other_data.image_data.get() + size_their, their_dels.data(), their_dels.data() + their_dels.size());
	BOOST_TEST_MESSAGE("Our output histogram: ");
	for (size_t i = 0; i < bins_count; i++)
	{
		BOOST_TEST_MESSAGE("Range<" << our_dels[i] << ", " << our_dels[i+1] << ">: " << our_bins[i]);
	}
	BOOST_TEST_MESSAGE("Not in delimeters: " << our_bins[bins_count]);
	BOOST_TEST_MESSAGE("NaNs: " << our_bins[bins_count + 1]);
	BOOST_TEST_MESSAGE("Their output histogram: ");
	for (size_t i = 0; i < bins_count; i++)
	{
		BOOST_TEST_MESSAGE("Range<" << their_dels[i] << ", " << their_dels[i + 1] << ">: " << their_bins[i]);
	}
	BOOST_TEST_MESSAGE("Not in delimeters: " << their_bins[bins_count]);
	BOOST_TEST_MESSAGE("NaNs: " << their_bins[bins_count + 1]);
}


BOOST_AUTO_TEST_CASE(real_data_old_spiral_with_rectangle)
{
	using ref_tar_t = tuple<string_view, string_view, string_view>;
	array<ref_tar_t, 1> image_pairs =
	{
		ref_tar_t{"data/raw/Ref.raw", "data/raw/Eval.raw", "data/mhd/comp_Ref.mhd_Eval.mhd.out"},
	};

	auto prefix_local = "spiral_results_local_"s;
	auto prefix_global = "spiral_results_global_"s;

	BOOST_TEST_MESSAGE("Spiral solver");

	for (auto& ref_tar : image_pairs)
	{
		BOOST_TEST_MESSAGE("Reference: " << get<0>(ref_tar));
		BOOST_TEST_MESSAGE("Target: " << get<1>(ref_tar));
		BOOST_TEST_MESSAGE("Other tool output: " << get<2>(ref_tar));

		auto raw_reference = load_raw_dataset(get<0>(ref_tar));
		auto raw_target = load_raw_dataset(get<1>(ref_tar));
		auto raw_output = load_raw_output(get<2>(ref_tar));

		BOOST_ASSERT(raw_reference && raw_target && raw_output);

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
			if (results_global)
				BOOST_TEST_MESSAGE("Time taken global: " << results_global->time_taken.count() / 1e9 << "s");

			if (!results_local)// || !save_results(filesystem::path(prefix_local + string(ref_tar.first)), *results_local))
				BOOST_TEST_MESSAGE("Failed to calculate!");
			if (!results_global)// || !save_results(filesystem::path(prefix_global + string(ref_tar.first)), *results_global))
				BOOST_TEST_MESSAGE("Failed to calculate!");

			std::visit([&](auto&& td) { compare_results(*results_local, td); }, *raw_output);
			std::visit([&](auto&& td) { compare_results(*results_global, td); }, *raw_output);
		}
		else
		{
			auto results_local = run_gi_spiral_rectangle<local_gamma_index_params>(get<1>(prepared_reference), get<1>(prepared_target), get<1>(distance_to_agreement), percentage_d);
			if (results_local)
				BOOST_TEST_MESSAGE("Time taken local: " << results_local->time_taken.count() / 1e9 << "s");
			auto results_global = run_gi_spiral_rectangle<global_gamma_index_params>(get<1>(prepared_reference), get<1>(prepared_target), get<1>(distance_to_agreement), get<1>(normalization_dose));
			if (results_global)
				BOOST_TEST_MESSAGE("Time taken global: " << results_global->time_taken.count() / 1e9 << "s");

			if (!results_local)// || !save_results(filesystem::path(prefix_local + string(ref_tar.first)), *results_local))
				BOOST_TEST_MESSAGE("Failed to calculate!");
			if (!results_global)// || !save_results(filesystem::path(prefix_global + string(ref_tar.first)), *results_global))
				BOOST_TEST_MESSAGE("Failed to calculate!");

			std::visit([&](auto&& td) { compare_results(*results_local, td); }, *raw_output);
			std::visit([&](auto&& td) { compare_results(*results_global, td); }, *raw_output);
		}
	}
}

BOOST_AUTO_TEST_CASE(real_data_new_method)
{
	using ref_tar_t = tuple<string_view, string_view, string_view>;
	array<ref_tar_t, 1> image_pairs =
	{
		ref_tar_t{"data/raw/Ref.raw", "data/raw/Eval.raw", "data/mhd/comp_Ref.mhd_Eval.mhd.out"},
	};
	
	auto prefix_local = "new_method_results_local_"s;
	auto prefix_global = "new_method_results_global_"s;

	BOOST_TEST_MESSAGE("Vectorized MT");

	for (auto& ref_tar : image_pairs)
	{
		BOOST_TEST_MESSAGE("Reference: " << get<0>(ref_tar));
		BOOST_TEST_MESSAGE("Target: " << get<1>(ref_tar));
		BOOST_TEST_MESSAGE("Other tool output: " << get<2>(ref_tar));

		auto raw_reference = load_raw_dataset(get<0>(ref_tar));
		auto raw_target = load_raw_dataset(get<1>(ref_tar));
		auto raw_output = load_raw_output(get<2>(ref_tar));

		BOOST_ASSERT(raw_reference && raw_target && raw_output);

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
			if (results_global)
				BOOST_TEST_MESSAGE("Time taken global: " << results_global->time_taken.count() / 1e9 << "s");

			if (!results_local)// || !save_results(filesystem::path(prefix_local + string(ref_tar.first)), *results_local))
				BOOST_TEST_MESSAGE("Failed to calculate!");
			if (!results_global)// || !save_results(filesystem::path(prefix_global + string(ref_tar.first)), *results_global))
				BOOST_TEST_MESSAGE("Failed to calculate!");

			std::visit([&](auto&& td) { compare_results(*results_local, td); }, *raw_output);
			std::visit([&](auto&& td) { compare_results(*results_global, td); }, *raw_output);
		}
		else
		{
			auto results_local = run_gi<local_gamma_index_params>(get<1>(prepared_reference), get<1>(prepared_target), get<1>(distance_to_agreement), percentage_d);
			if(results_local)
				BOOST_TEST_MESSAGE("Time taken local: " << results_local->time_taken.count()/1e9 << "s");
			auto results_global = run_gi<global_gamma_index_params>(get<1>(prepared_reference), get<1>(prepared_target), get<1>(distance_to_agreement), get<1>(normalization_dose));
			if (results_global)
				BOOST_TEST_MESSAGE("Time taken global: " << results_global->time_taken.count() / 1e9 << "s");

			if (!results_local)// || !save_results(filesystem::path(prefix_local + string(ref_tar.first)), *results_local))
				BOOST_TEST_MESSAGE("Failed to calculate!");
			if (!results_global)// || !save_results(filesystem::path(prefix_global + string(ref_tar.first)), *results_global))
				BOOST_TEST_MESSAGE("Failed to calculate!");

			std::visit([&](auto&& td) { compare_results(*results_local, td); }, *raw_output);
			std::visit([&](auto&& td) { compare_results(*results_global, td); }, *raw_output);
		}
	}
}*/