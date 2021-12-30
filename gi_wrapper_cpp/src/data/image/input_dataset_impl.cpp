#include <data/image/input_dataset_impl.hpp>

#include <data/image/float32_image.hpp>
#include <data/image/float64_image.hpp>

#include <gdcmImageReader.h>

namespace yagit::gdcm::data
{
	constexpr uint16_t grid_dimensions_group = 0x0064;
	constexpr uint16_t grid_dimensions_element = 0x0064;

	float32_input_rtdose_dataset open_dicom_r_f32(const fs::path& path, error_code& ec)
	{
		ec = {};
		::gdcm::ImageReader reader;
		reader.SetFileName(fs::absolute(path).string().c_str());
		if (!reader.Read()) throw std::runtime_error("Invalid file!");
		auto& image = reader.GetImage();
		ycd::sizes<3> grid_dimensions = {
			image.GetDimension(0),
			image.GetDimension(1),
			image.GetDimension(2)
		};

		if (grid_dimensions.sizes[2] != 0)
			return std::make_unique<dataset_input_file_mapping<float, 3>>(path, grid_dimensions);
		else if (grid_dimensions.sizes[1] != 0)
			return std::make_unique<dataset_input_file_mapping<float, 2>>(path, ycd::sizes<2>{grid_dimensions.sizes[0], grid_dimensions.sizes[1]});
		else
			return std::make_unique<dataset_input_file_mapping<float, 1>>(path, ycd::sizes<1>{grid_dimensions.sizes[0]});
	}

	float64_input_rtdose_dataset open_dicom_r_f64(const fs::path& path, error_code& ec)
	{
		ec = {};
		::gdcm::ImageReader reader;
		reader.SetFileName(fs::absolute(path).string().c_str());
		if (!reader.Read()) throw std::runtime_error("Invalid file!");
		auto& image = reader.GetImage();
		ycd::sizes<3> grid_dimensions = {
			image.GetDimension(0),
			image.GetDimension(1),
			image.GetDimension(2)
		};

		if (grid_dimensions.sizes[2] != 0)
			return std::make_unique<dataset_input_file_mapping<double, 3>>(path, grid_dimensions);
		else if (grid_dimensions.sizes[1] != 0)
			return std::make_unique<dataset_input_file_mapping<double, 2>>(path, ycd::sizes<2>{grid_dimensions.sizes[0], grid_dimensions.sizes[1]});
		else
			return std::make_unique<dataset_input_file_mapping<double, 1>>(path, ycd::sizes<1>{grid_dimensions.sizes[0]});
	}
}