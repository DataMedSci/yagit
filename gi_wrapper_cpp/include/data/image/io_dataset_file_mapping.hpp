#pragma once

#include <data/image/iwritable_image.hpp>
#include <data/image/rtdose/irtdose_image.hpp>
#include <filesystem>

namespace yagit::gdcm::data
{
    namespace ycd = yagit::core::data;
    namespace fs = std::filesystem;

    using std::variant;
    using std::optional;
    using std::error_code;
    using std::unique_ptr;

    template<typename ElementType, size_t Dimensions>
    class idataset_output_file_mapping
    {
    public:
        virtual ~idataset_output_file_mapping() = default;
    public:
        virtual ycd::iwritable_image<ElementType, Dimensions>& get_image() = 0;
        virtual error_code save_to_disk() = 0;
    };

    template<typename ElementType, size_t Dimensions>
    class irtdose_dataset_input_file_mapping
    {
    public:
        virtual ~irtdose_dataset_input_file_mapping() = default;
    public:
        virtual const ycd::irtdose_image<ElementType, Dimensions>& get_image() = 0;
    };

    template<size_t Dimensions>
    using float32_output_dataset = unique_ptr<idataset_output_file_mapping<float, Dimensions>>;
    template<size_t Dimensions>
    using float64_output_dataset = unique_ptr<idataset_output_file_mapping<double, Dimensions>>;

    using float32_input_rtdose_dataset = optional<variant<
            unique_ptr<irtdose_dataset_input_file_mapping<float, 1>>,
            unique_ptr<irtdose_dataset_input_file_mapping<float, 2>>,
            unique_ptr<irtdose_dataset_input_file_mapping<float, 3>>>>;
    using float64_input_rtdose_dataset = optional<variant<
            unique_ptr<irtdose_dataset_input_file_mapping<double, 1>>,
            unique_ptr<irtdose_dataset_input_file_mapping<double, 2>>,
            unique_ptr<irtdose_dataset_input_file_mapping<double, 3>>>>;
}