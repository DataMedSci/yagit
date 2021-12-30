#pragma once

#include <data/image/io_dataset_file_mapping.hpp>

namespace yagit::gdcm::data
{
    namespace detail
    {
        float32_output_dataset<1> open_dicom_w_1D_f32(const fs::path& path, error_code& ec, const ycd::sizes<1>& size);
        float32_output_dataset<2> open_dicom_w_2D_f32(const fs::path& path, error_code& ec, const ycd::sizes<2>& size);
        float32_output_dataset<3> open_dicom_w_3D_f32(const fs::path& path, error_code& ec, const ycd::sizes<3>& size);
    }

    float32_input_rtdose_dataset open_dicom_r_f32(const fs::path& path, error_code& ec);
    template<size_t Dimensions>
    float32_output_dataset<Dimensions> open_dicom_w_f32(const fs::path& path, error_code& ec, const ycd::sizes<Dimensions>& size)
    {
        static_assert(Dimensions > 0 && Dimensions <= 3, "Only 1,2,3 dimensions are supported!");
        if constexpr(Dimensions == 1)
            return detail::open_dicom_w_1D_f32(path, ec, size);
        if constexpr(Dimensions == 2)
            return detail::open_dicom_w_2D_f32(path, ec, size);
        if constexpr(Dimensions == 3)
            return detail::open_dicom_w_3D_f32(path, ec, size);
        return nullptr;
    }
}