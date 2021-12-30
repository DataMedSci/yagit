#pragma once

#include <data/image/io_dataset_file_mapping.hpp>
#include <data/image/input_conversion_helpers.hpp>
#include <gdcmImageRegionReader.h>
#include <gdcmBoxRegion.h>
#include <gdcmAttribute.h>

namespace yagit::gdcm::data
{
    constexpr uint16_t image_position_group = 0x0020;
    constexpr uint16_t image_position_element = 0x0032;

    constexpr uint16_t pixel_spacing_group = 0x0028;
    constexpr uint16_t pixel_spacing_element = 0x0030;

    constexpr uint16_t dcm_modality_group = 0x0008;
    constexpr uint16_t dcm_modality_element = 0x0060;
    constexpr auto rtdose_modality_value = "RTDOSE";

    constexpr uint16_t dcm_spacing_between_slices_group = 0x0018;
    constexpr uint16_t dcm_spacing_between_slices_element = 0x0088;

    constexpr uint16_t dcm_grid_frame_offset_vector_group = 0x3004;
    constexpr uint16_t dcm_grid_frame_offset_vector_element = 0x000C;

    template<typename ElementType, size_t Dimensions>
    class input_image_region
        : public virtual ycd::irtdose_image_region<ElementType, Dimensions>
    {
    protected:
        ::gdcm::ImageRegionReader& _image_region_reader;
        ycd::sizes<Dimensions> _original_size;
        ycd::data_region<Dimensions> _region;
    public:
        input_image_region(::gdcm::ImageRegionReader& region_reader, const ycd::sizes<Dimensions>& original_size, const ycd::data_region <Dimensions>& region)
            : _image_region_reader(region_reader)
            , _original_size(original_size)
            , _region(region)
        {}
    public:
        virtual ~input_image_region() = default;
    public:
        virtual ycd::data_format<Dimensions> preferred_data_format() const override
        {
            return ycd::default_format<Dimensions>;
        }
        virtual ycd::data_region<Dimensions> region() const noexcept override
        {
            return _region;
        }
    protected:
        virtual error_code load(ElementType* data_storage, const ycd::data_format<Dimensions>& format, size_t& required_size) const override
        {
            if (data_storage == nullptr)
            {
                required_size = ycd::total_size(region().size);
            }
            else
            {
                if (required_size != ycd::total_size(region().size))
                    return {}; // error not enough space allocated

                ::gdcm::BoxRegion gdcm_region;
                if constexpr (Dimensions == 1)
                    gdcm_region.SetDomain(
                        static_cast<uint32_t>(_region.offset.offset[0]), static_cast<uint32_t>(_region.offset.offset[0] + _region.size.sizes[0]),
                        0, 0,
                        0, 0);
                else if constexpr(Dimensions == 2)
                    gdcm_region.SetDomain(
                        static_cast<uint32_t>(_region.offset.offset[0]), static_cast<uint32_t>(_region.offset.offset[0] + _region.size.sizes[0]),
                        static_cast<uint32_t>(_region.offset.offset[1]), static_cast<uint32_t>(_region.offset.offset[1] + _region.size.sizes[1]),
                        0, 0);
                else
                    gdcm_region.SetDomain(
                        static_cast<uint32_t>(_region.offset.offset[0]), static_cast<uint32_t>(_region.offset.offset[0] + _region.size.sizes[0]),
                        static_cast<uint32_t>(_region.offset.offset[1]), static_cast<uint32_t>(_region.offset.offset[1] + _region.size.sizes[1]),
                        static_cast<uint32_t>(_region.offset.offset[2]), static_cast<uint32_t>(_region.offset.offset[2] + _region.size.sizes[2]));
                
                _image_region_reader.SetRegion(gdcm_region);

                ::gdcm::Image& image = _image_region_reader.GetImage();

                auto buffer_length = image.GetBufferLength();
                auto buf_len_max_align_t = buffer_length / sizeof(max_align_t) + (buffer_length % sizeof(max_align_t) > 0 ? 1 : 0);
                unique_ptr<std::byte[]> buffer = unique_ptr<std::byte[]>(reinterpret_cast<std::byte*>(new max_align_t[buf_len_max_align_t]));

                _image_region_reader.ReadIntoBuffer(reinterpret_cast<char*>(buffer.get()), buffer_length);

                if (auto ec = transform_convert(data_storage, buffer.get(), required_size, image.GetPixelFormat()))
                    return ec;

                ElementType rescaleSlope = static_cast<ElementType>(image.GetSlope());
                ElementType rescaleIntercept = static_cast<ElementType>(image.GetIntercept());

                std::transform(data_storage, data_storage + required_size, data_storage, [rescaleSlope, rescaleIntercept](auto&& v)
                    {
                        return v * rescaleSlope + rescaleIntercept;
                    });
            }

            return {}; // ok
        }
        virtual ycd::irtdose_image_region<ElementType, Dimensions - 1>* create_slice(size_t dimension, size_t index, error_code &ec) const override
        {
            ec = {};
            return nullptr; // not implemented
        }
        virtual ycd::data_format<Dimensions> preferred_coordinates_format(size_t dimension) const override
        {
            return ycd::default_format<Dimensions>;
        }
        virtual error_code load_coordinates(ElementType* data_storage, size_t dimension, const ycd::data_format<Dimensions>& format, size_t& required_size) const override
        {
            if (data_storage == nullptr)
            {
                required_size = ycd::total_size(region().size);
            }
            else
            {
                if (required_size != ycd::total_size(region().size))
                    return {}; // error not enough space allocated

                auto& dataset = _image_region_reader.GetFile().GetDataSet();

                auto start = get_start(dataset);
                auto spacing = get_spacing(dataset);
                std::array<ElementType, Dimensions> region_start;
                for (size_t i = 0; i < Dimensions; i++)
                    region_start[i] = start[i] + spacing[i] * _region.offset.offset[i];

                if (format != preferred_coordinates_format(dimension))
                    return {}; // TODO implement other formats

                if constexpr (Dimensions == 1)
                {
                    for (size_t x = 0; x < _region.size.sizes[0]; ++x, ++data_storage)
                        *data_storage = region_start[0] + spacing[0] * x;
                }
                else if constexpr (Dimensions == 2)
                {
                    if (dimension == 0) // x-dimension (rows)
                    {
                        for (size_t x = 0; x < _region.size.sizes[0]; ++x)
                        {
                            auto ds_end = data_storage + _region.size.sizes[1];
                            std::fill(data_storage, ds_end, region_start[0] + spacing[0] * x);
                            data_storage = ds_end;
                        }
                    }
                    else // y-dimension (columns)
                    {
                        for (size_t x = 0; x < _region.size.sizes[0]; ++x)
                        {
                            for (size_t y = 0; y < _region.size.sizes[1]; ++y)
                            {
                                *data_storage++ = region_start[1] + spacing[1] * y;
                            }
                        }
                    }
                }
                else
                {
                    if (dimension == 0) // x-dimension (rows)
                    {
                        for (size_t x = 0; x < _region.size.sizes[0]; ++x)
                        {
                            for (size_t y = 0; y < _region.size.sizes[1]; ++y)
                            {
                                auto ds_end = data_storage + _region.size.sizes[2];
                                std::fill(data_storage, ds_end, region_start[0] + spacing[0] * x);
                                data_storage = ds_end;
                            }
                        }
                    }
                    else if (dimension == 1) // y-dimension (columns)
                    {
                        for (size_t x = 0; x < _region.size.sizes[0]; ++x)
                        {
                            for (size_t y = 0; y < _region.size.sizes[1]; ++y)
                            {
                                auto ds_end = data_storage + _region.size.sizes[2];
                                std::fill(data_storage, ds_end, region_start[1] + spacing[1] * y);
                                data_storage = ds_end;
                            }
                        }
                    }
                    else // z-dimension
                    {
                        for (size_t x = 0; x < _region.size.sizes[0]; ++x)
                        {
                            for (size_t y = 0; y < _region.size.sizes[1]; ++y)
                            {
                                for (size_t z = 0; z < _region.size.sizes[2]; ++z)
                                {
                                    *data_storage++ = region_start[2] + spacing[2] * z;
                                }
                            }
                        }
                    }
                }
            }

            return {};
        }
        virtual ycd::irtdose_image_region<ElementType, Dimensions>* create_subregion(const ycd::data_region<Dimensions>& region, error_code& ec) const override
        {
            ycd::data_region <Dimensions> new_region = {{}, region.size};
            for (size_t i = 0; i < Dimensions; i++)
            {
                auto region_offset = _region.offset.offset[i];
                auto region_end = region_offset + _region.size.sizes[i];
                auto new_region_offset = region.offset.offset[i] + region_offset;
                auto new_region_end = new_region_offset + region.size.sizes[i];
                new_region.offset.offset[i] = new_region_offset;
                if (!(new_region_offset >= region_offset && new_region_offset < region_end) ||
                    !(new_region_end >= region_offset && new_region_end < region_end)
                        )
                {
                    ec = {}; // invalid region - outside bounds
                    return nullptr;
                }
            }

            return new input_image_region(_image_region_reader, _original_size, new_region);
        }
    private:
        std::array<ElementType, Dimensions> get_start(const ::gdcm::DataSet& dataset) const
        {
            ::gdcm::Attribute<image_position_group, image_position_element> image_position;
            image_position.SetFromDataSet(dataset);

            if constexpr (Dimensions == 1)
            {
                return {
                    static_cast<ElementType>(image_position.GetValue(0))
                };
            }
            else if constexpr (Dimensions == 2)
            {
                return {
                    static_cast<ElementType>(image_position.GetValue(0)),
                    static_cast<ElementType>(image_position.GetValue(1))
                };
            }
            else
            {
                return {
                    static_cast<ElementType>(image_position.GetValue(0)),
                    static_cast<ElementType>(image_position.GetValue(1)),
                    static_cast<ElementType>(image_position.GetValue(2))
                };
            }
        }
        std::array<ElementType, Dimensions> get_spacing(const ::gdcm::DataSet& dataset) const
        {
            ::gdcm::Attribute<pixel_spacing_group, pixel_spacing_element> pixel_spacing;
            pixel_spacing.SetFromDataSet(dataset);

            // We subtract dim from 1 because the order of pixel spacing values is inversed in DICOM standard.
            // More info - http://dicom.nema.org/medical/Dicom/2016b/output/chtml/part03/sect_10.7.html.

            if constexpr (Dimensions == 1)
            {
                return {
                    static_cast<ElementType>(pixel_spacing.GetValue(0))
                };
            }
            else if constexpr (Dimensions == 2)
            {
                return {
                    static_cast<ElementType>(pixel_spacing.GetValue(1)),
                    static_cast<ElementType>(pixel_spacing.GetValue(0))
                };
            }
            else
            {
                ElementType third_dim_spacing = 0;

                ::gdcm::Attribute<dcm_modality_group, dcm_modality_element> modality;
                modality.SetFromDataSet(dataset);

                if (modality.GetValue() == rtdose_modality_value &&
                    dataset.FindDataElement(::gdcm::Tag(dcm_grid_frame_offset_vector_group, dcm_grid_frame_offset_vector_element)))
                {
                    ::gdcm::Attribute<dcm_grid_frame_offset_vector_group, dcm_grid_frame_offset_vector_element> grid_frame_offset_vector;
                    grid_frame_offset_vector.SetFromDataSet(dataset);

                    third_dim_spacing = static_cast<ElementType>(grid_frame_offset_vector.GetValue(1)) - static_cast<ElementType>(grid_frame_offset_vector.GetValue(0));
                }

                if (std::abs(third_dim_spacing) <= std::numeric_limits<ElementType>::epsilon() &&
                    dataset.FindDataElement(::gdcm::Tag(dcm_spacing_between_slices_group, dcm_spacing_between_slices_element)))
                {
                    ::gdcm::Attribute<dcm_spacing_between_slices_group, dcm_spacing_between_slices_element> spacing_between_slices;
                    spacing_between_slices.SetFromDataSet(dataset);
                    third_dim_spacing = static_cast<ElementType>(spacing_between_slices.GetValue());
                }

                return {
                    static_cast<ElementType>(pixel_spacing.GetValue(1)),
                    static_cast<ElementType>(pixel_spacing.GetValue(0)),
                    third_dim_spacing
                };
            }
        }
    };

    template<typename ElementType, size_t Dimensions>
    class input_image
            : public input_image_region<ElementType, Dimensions>
            , public ycd::irtdose_image<ElementType, Dimensions>
    {
    public:
        input_image(::gdcm::ImageRegionReader& region_reader, const ycd::sizes<Dimensions>& image_size)
            : input_image_region<ElementType, Dimensions>(region_reader, image_size, ycd::data_region<Dimensions>{ycd::zero_offset<Dimensions>, image_size})
        {}
    public:
        virtual ~input_image() = default;
    public:
        virtual ycd::sizes <Dimensions> size() const override
        {
            return input_image_region<ElementType, Dimensions>::_original_size;
        }
    };

    template<typename ElementType, size_t Dimensions>
    class dataset_input_file_mapping
        : public irtdose_dataset_input_file_mapping<ElementType, Dimensions>
    {
    private:
        ::gdcm::ImageRegionReader _image_region_reader;
        input_image<ElementType, Dimensions> _image;
    public:
        dataset_input_file_mapping(const fs::path& path, const ycd::sizes<Dimensions>& image_size)
            : _image(_image_region_reader, image_size)
        {
            _image_region_reader.SetFileName(fs::absolute(path).string().c_str());
        }
    public:
        virtual ~dataset_input_file_mapping() override = default;
    public:
        virtual const ycd::irtdose_image<ElementType, Dimensions>& get_image() override
        {
            return _image;
        }
    };
}