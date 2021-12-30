#pragma once

#include <data/image/io_dataset_file_mapping.hpp>
#include <gdcmImageWriter.h>

namespace yagit::gdcm::data
{
    template<typename ElementType, size_t Dimensions>
    class output_image_region
            : public ycd::iwritable_image_region<ElementType, Dimensions>
    {
    private:
        core::view<ElementType> _data;
        ycd::sizes<Dimensions> _original_size;
        ycd::data_region<Dimensions> _region;
    public:
        output_image_region(core::view<ElementType> data, const ycd::sizes<Dimensions>& original_size, const ycd::data_region<Dimensions>& region)
            : _data(data)
            , _original_size(original_size)
            , _region(region)
        {}
    public:
        virtual ~output_image_region() = default;
    public:
        virtual ycd::data_format<Dimensions> preferred_data_format() const override
        {
            return ycd::default_format<Dimensions>;
        }
        virtual ycd::data_region<Dimensions> region() const noexcept override
        {
            return _region;
        }
        virtual error_code save(const ycd::image_data<ElementType, Dimensions>& data) override
        {
            if(data.dimension_sizes() != region().size)
            {
                return {}; // error invalid region - sizes dont match
            }

            ycd::copy(
                    _data, region(), _original_size, preferred_data_format(),
                    data.data(), ycd::data_region<Dimensions>{ycd::zero_offset<Dimensions>, data.dimension_sizes()},
                    data.dimension_sizes(), data.format());

            return {}; // ok
        }
    protected:
        virtual error_code load(ElementType* data_storage, const ycd::data_format<Dimensions>& format, size_t& required_size) const override
        {
            if(data_storage == nullptr)
            {
                required_size = ycd::total_size(region().size);
            }
            else
            {
                if(required_size != ycd::total_size(region().size))
                    return {}; // error not enough space allocated

                ycd::copy(data_storage, ycd::data_region<Dimensions>{ycd::zero_offset<Dimensions>, region().size}, region().size, format,
                          _data, region(), _original_size, preferred_data_format());
            }

            return {}; // ok
        }
        virtual ycd::iwritable_image_region<ElementType, Dimensions - 1>* create_slice(size_t dimension, size_t index, error_code& ec) const override
        {
            ec = {};
            return nullptr; // not implemented yet
        }
        virtual ycd::iwritable_image_region<ElementType, Dimensions>* create_subregion(const ycd::data_region<Dimensions>& region, error_code& ec) const override
        {
            ycd::data_region<Dimensions> new_region = {{}, region.size};
            for(size_t i = 0; i < Dimensions; i++)
            {
                auto region_offset = _region.offset.offset[i];
                auto region_end = region_offset + _region.size.sizes[i];
                auto new_region_offset = region.offset.offset[i] + region_offset;
                auto new_region_end = new_region_offset + region.size.sizes[i];
                new_region.offset.offset[i] = new_region_offset;
                if(!(new_region_offset >= region_offset && new_region_offset < region_end) ||
                    !(new_region_end >= region_offset && new_region_end < region_end)
                    )
                {
                    ec = {}; // invalid region - outside bounds
                    return nullptr;
                }
            }

            return new output_image_region(_data, _original_size, new_region);
        }
    };

    template<typename ElementType, size_t Dimensions>
    class output_image
            : public ycd::iwritable_image<ElementType, Dimensions>
    {
    private:
        core::view<ElementType> _data;
        ycd::sizes<Dimensions> _size;
    public:
        output_image(core::view<ElementType> data, const ycd::sizes<Dimensions>& size)
            : _data(data)
            , _size(size)
        {}
    public:
        virtual ~output_image() override = default;
    public:
        virtual ycd::sizes<Dimensions> size() const override
        {
            return _size;
        }
        virtual ycd::data_format<Dimensions> preferred_data_format() const override
        {
            return ycd::default_format<Dimensions>;
        }
        virtual ycd::data_region<Dimensions> region() const noexcept override
        {
            return {ycd::zero_offset<Dimensions>, size()};
        }
        virtual error_code save(const ycd::image_data<ElementType, Dimensions>& data) override
        {
            if(data.dimension_sizes() != region().size)
            {
                return {}; // error invalid region - sizes dont match
            }

            ycd::copy(
                    _data, region(), size(), preferred_data_format(),
                    data.data(), ycd::data_region<Dimensions>{ycd::zero_offset<Dimensions>,
                    data.dimension_sizes()}, data.dimension_sizes(), data.format());

            return {}; // ok
        }
    protected:
        virtual error_code load(ElementType* data_storage, const ycd::data_format<Dimensions>& format, size_t& required_size) const override
        {
            if(data_storage == nullptr)
            {
                required_size = ycd::total_size(size());
            }
            else
            {
                if(required_size != ycd::total_size(size()))
                    return {}; // error not enough space allocated

                ycd::copy(data_storage, ycd::data_region<Dimensions>{ycd::zero_offset<Dimensions>, size()}, size(),format,
                          _data, region(), size(), preferred_data_format());
            }

            return {}; // ok
        }
        virtual ycd::iwritable_image_region<ElementType, Dimensions - 1>* create_slice(size_t dimension, size_t index, error_code& ec) const override
        {
            ec = {};
            return nullptr; // not implemented yet
        }
        virtual ycd::iwritable_image_region<ElementType, Dimensions>* create_subregion(const ycd::data_region<Dimensions>& region, error_code& ec) const override
        {
            ycd::data_region<Dimensions> new_region = {{}, region.size};
            for(size_t i = 0; i < Dimensions; i++)
            {
                auto region_offset = 0;
                auto region_end = region_offset + _size.sizes[i];
                auto new_region_offset = region.offset.offset[i] + region_offset;
                auto new_region_end = new_region_offset + region.size.sizes[i];
                new_region.offset.offset[i] = new_region_offset;
                if(!(new_region_offset >= region_offset && new_region_offset < region_end) ||
                   !(new_region_end >= region_offset && new_region_end < region_end)
                        )
                {
                    ec = {}; // invalid region - outside bounds
                    return nullptr;
                }
            }

            return new output_image_region(_data, _size, new_region);
        }
    };

    constexpr uint16_t pixel_data_group = 0x7FE0;
    constexpr uint16_t pixel_data_element = 0x0010;

    template<typename ElementType, size_t Dimensions>
    class dataset_output_file_mapping
            : public idataset_output_file_mapping<ElementType, Dimensions>
    {
    private:
        ::gdcm::ImageWriter _imageWriter;
        unique_ptr<ElementType[]> _data;
        unique_ptr<output_image<ElementType, Dimensions>> _image;
    public:
        dataset_output_file_mapping(
                const fs::path& path, const ycd::sizes<Dimensions>& size)
            : _imageWriter()
            , _data(std::make_unique<ElementType[]>(ycd::total_size(size)))
            , _image(std::make_unique<output_image<ElementType, Dimensions>>(_data.get(), size))
        {
            ::gdcm::ImageWriter writer;
            writer.SetFileName(fs::absolute(path).string().c_str());
            if (!writer.Write()) throw std::runtime_error("Couldn't open specified file");
            ::gdcm::Image& image = writer.GetImage();
            image.SetNumberOfDimensions(Dimensions);
            for(size_t i = 0; i < Dimensions; i++)
                image.SetDimension(static_cast<uint32_t>(i), static_cast<uint32_t>(size.sizes[i]));

            ::gdcm::DataElement pixel_data(::gdcm::Tag(pixel_data_group, pixel_data_element));
            pixel_data.SetByteValue(reinterpret_cast<const char*>(_data.get()), static_cast<uint32_t>(ycd::total_size(size)));
            image.SetDataElement(pixel_data);
        }
    public:
        virtual ~dataset_output_file_mapping() = default;
    public:
        virtual ycd::iwritable_image<ElementType, Dimensions>& get_image() override
        {
            return *_image.get();
        }
        virtual error_code save_to_disk() override
        {
            if(!_imageWriter.Write())
            {
                return {}; // error failed to write
            }

            return {}; // ok
        }
    };
}