#include <data/image/output_dataset_impl.hpp>

#include <data/image/float32_image.hpp>
#include <data/image/float64_image.hpp>

namespace yagit::gdcm::data::detail
{
    float32_output_dataset<1> open_dicom_w_1D_f32(const fs::path& path, error_code& ec, const ycd::sizes<1>& size)
    {
        ec = {};
        return float32_output_dataset<1>(new dataset_output_file_mapping<float, 1>(path, size));
    }
    float32_output_dataset<2> open_dicom_w_2D_f32(const fs::path& path, error_code& ec, const ycd::sizes<2>& size)
    {
        ec = {};
        return float32_output_dataset<2>(new dataset_output_file_mapping<float, 2>(path, size));
    }
    float32_output_dataset<3> open_dicom_w_3D_f32(const fs::path& path, error_code& ec, const ycd::sizes<3>& size)
    {
        ec = {};
        return float32_output_dataset<3>(new dataset_output_file_mapping<float, 3>(path, size));
    }

    float64_output_dataset<1> open_dicom_w_1D_f64(const fs::path& path, error_code& ec, const ycd::sizes<1>& size)
    {
        ec = {};
        return float64_output_dataset<1>(new dataset_output_file_mapping<double, 1>(path, size));
    }
    float64_output_dataset<2> open_dicom_w_2D_f64(const fs::path& path, error_code& ec, const ycd::sizes<2>& size)
    {
        ec = {};
        return float64_output_dataset<2>(new dataset_output_file_mapping<double, 2>(path, size));
    }
    float64_output_dataset<3> open_dicom_w_3D_f64(const fs::path& path, error_code& ec, const ycd::sizes<3>& size)
    {
        ec = {};
        return float64_output_dataset<3>(new dataset_output_file_mapping<double, 3>(path, size));
    }
}