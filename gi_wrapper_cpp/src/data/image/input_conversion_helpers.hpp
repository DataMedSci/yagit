#pragma once

#include <data/image/io_dataset_file_mapping.hpp>

#include <gdcmPixelFormat.h>

#include <bit>
#include <cstdint>
#include <type_traits>
#include <cstring>

namespace yagit::gdcm::data
{
    namespace detail
    {
        template<size_t ByteBlockSize>
        struct byte_block { std::byte block[ByteBlockSize]; };

        struct half_t
        {
            uint16_t _pd;
        };
        struct int12_t
        {
            int16_t _pd;
        };
        struct uint12_t
        {
            uint16_t _pd;
        };

        template<typename Type>
        struct special_type : std::false_type
        {
            using storage_type = Type;
            using preparation_type = Type;
        };
        template<> struct special_type<half_t> : std::true_type
        {
            using storage_type = float;
            using preparation_type = uint16_t;
        };
        template<> struct special_type<int12_t> : std::true_type
        {
            using storage_type = int16_t;
            using preparation_type = int16_t;
        };
        template<> struct special_type<uint12_t> : std::true_type
        {
            using storage_type = uint16_t;
            using preparation_type = uint16_t;
        };

        template<typename T>
        using preparation_type_t = typename special_type<T>::preparation_type;
        template<typename T>
        using storage_type_t = typename special_type<T>::storage_type;

        template<size_t TargetByteBlockSize, size_t ByteBlockSize, typename = std::enable_if_t<TargetByteBlockSize <= ByteBlockSize>>
        inline const byte_block<TargetByteBlockSize>& extract_first(const byte_block<ByteBlockSize>& bb)
        {
            return *reinterpret_cast<const byte_block<TargetByteBlockSize>*>(&bb);
        }

        template <class To, class From>
        std::enable_if_t<
                sizeof(To) == sizeof(From) &&
                std::is_trivially_copyable_v<From> &&
                std::is_trivially_copyable_v<To>,
                To>
        bit_cast(const From& src) noexcept
        {
            static_assert(std::is_trivially_constructible_v<To>,
                          "This implementation additionally requires destination type to be trivially constructible");
            To dst;
            std::memcpy(&dst, &src, sizeof(To));
            return dst;
        }

        template<typename StoredType, size_t ByteBlockSize, typename = std::enable_if_t<ByteBlockSize >= sizeof(preparation_type_t<StoredType>)>>
        inline StoredType prepare_bb(const byte_block<ByteBlockSize>& bb)
        {
            return StoredType(bit_cast<preparation_type_t<StoredType>>(extract_first<sizeof(preparation_type_t<StoredType>)>(bb)));
        }

        template<typename T>
        struct extractor { static T extract(const T& pv) { return pv; } };

        template<>
        struct extractor<half_t>
        {
            static storage_type_t<half_t> extract(const half_t& pv)
            {
                uint16_t h = pv._pd;
                return bit_cast<float, uint32_t>(((h & 0x8000) << 16) | (((h & 0x7c00) + 0x1C000) << 13) | ((h & 0x03FF) << 13));
            }
        };

        template<>
        struct extractor<int12_t>
        {
            static storage_type_t<int12_t> extract(const int12_t& pv)
            {
                return (pv._pd & 0x0800) ? (pv._pd & 0x0FFF | 0xF800) : pv._pd & 0x0FFF;
            }
        };

        template<>
        struct extractor<uint12_t>
        {
            static storage_type_t<uint12_t> extract(const uint12_t& pv)
            {
                return pv._pd & static_cast<uint16_t>(0x0FFF);
            }
        };

        template<typename TargetType, typename StoredType, size_t ByteBlockSize, typename = std::enable_if_t<ByteBlockSize >= sizeof(StoredType)>>
        inline TargetType as(const byte_block<ByteBlockSize>& bb)
        {
            return static_cast<TargetType>(extractor<StoredType>::extract(prepare_bb<StoredType>(bb)));
        }

        template<typename StoredType, typename ElementType, size_t ByteBlockSize>
        inline void transform_convert(core::view<ElementType> destination, core::const_view<byte_block<ByteBlockSize>> source, size_t element_count)
        {
            std::transform(source, source + element_count, destination, [](auto&& bb) {return as<ElementType, StoredType>(bb); });
        }

        template<size_t ByteBlockSize>
        inline core::const_view<byte_block<ByteBlockSize>> reinterpret_as_blocks(core::const_view<std::byte> source)
        {
            return reinterpret_cast<core::const_view<byte_block<ByteBlockSize>>>(source);
        }

        inline uint8_t left_rotate(uint8_t v, uint8_t count)
        {
            constexpr uint8_t uint8_bit_count = sizeof(uint8_t) * 8;
            return (v << count) | (v >> (uint8_bit_count - count));
        }

        template<typename TargetType, typename StoredType, size_t BitsAllocatedPerSample, size_t SamplesPerPixel>
        inline error_code transform_convert(core::view<TargetType> destination, core::const_view<std::byte> source, size_t element_count, size_t rgba_plane_index = 0)
        {
            if (rgba_plane_index >= SamplesPerPixel)
                return {}; // error invalid plane

            if constexpr (std::is_void_v<StoredType>)
            {
                auto de = destination + element_count;
                auto sit = reinterpret_cast<core::const_view<uint8_t>>(source);
                uint8_t bit_to_extract = 1 << rgba_plane_index;
                while (destination != de)
                {
                    *destination++ = static_cast<TargetType>((*sit++) & bit_to_extract);
                    bit_to_extract = left_rotate(bit_to_extract, SamplesPerPixel);
                }
            }
            else
            {
                constexpr size_t BytesPerSample = BitsAllocatedPerSample / 8;

                if constexpr (BytesPerSample < sizeof(StoredType))
                {
                    return {}; // error invalid stored type
                }
                else
                {
                    constexpr size_t SampleSizeInBytes = BytesPerSample * SamplesPerPixel;

                    transform_convert<StoredType>(destination, reinterpret_as_blocks<SampleSizeInBytes>(source + BytesPerSample * rgba_plane_index), element_count);
                }
            }

            return {}; // ok
        }

        template<typename TargetType>
        using transformation_f_t = error_code(*)(core::view<TargetType>, core::const_view<std::byte>, size_t, size_t);

        template<typename TargetType, size_t BitsAllocatedPerSample, size_t SamplesPerPixel>
        inline transformation_f_t<TargetType> transformation_f_selector(const ::gdcm::PixelFormat& pixelFormat, error_code& ec)
        {
            auto scalarType = pixelFormat.GetScalarType();

            switch (scalarType)
            {
// FLOATING POINT TYPES
            case ::gdcm::PixelFormat::ScalarType::FLOAT16:
                return transform_convert<TargetType, half_t, BitsAllocatedPerSample, SamplesPerPixel>;
            case ::gdcm::PixelFormat::ScalarType::FLOAT32:
                return transform_convert<TargetType, float, BitsAllocatedPerSample, SamplesPerPixel>;
            case ::gdcm::PixelFormat::ScalarType::FLOAT64:
                return transform_convert<TargetType, double, BitsAllocatedPerSample, SamplesPerPixel>;
// SIGNED INTEGRAL TYPES
            case ::gdcm::PixelFormat::ScalarType::INT8:
                return transform_convert<TargetType, int8_t, BitsAllocatedPerSample, SamplesPerPixel>;
            case ::gdcm::PixelFormat::ScalarType::INT12:
                return transform_convert<TargetType, int12_t, BitsAllocatedPerSample, SamplesPerPixel>;
            case ::gdcm::PixelFormat::ScalarType::INT16:
                return transform_convert<TargetType, int16_t, BitsAllocatedPerSample, SamplesPerPixel>;
            case ::gdcm::PixelFormat::ScalarType::INT32:
                return transform_convert<TargetType, int32_t, BitsAllocatedPerSample, SamplesPerPixel>;
            case ::gdcm::PixelFormat::ScalarType::INT64:
                return transform_convert<TargetType, int64_t, BitsAllocatedPerSample, SamplesPerPixel>;
// UNSIGNED INTEGRAL TYPES
            case ::gdcm::PixelFormat::ScalarType::UINT8:
                return transform_convert<TargetType, uint8_t, BitsAllocatedPerSample, SamplesPerPixel>;
            case ::gdcm::PixelFormat::ScalarType::UINT12:
                return transform_convert<TargetType, uint12_t, BitsAllocatedPerSample, SamplesPerPixel>;
            case ::gdcm::PixelFormat::ScalarType::UINT16:
                return transform_convert<TargetType, uint16_t, BitsAllocatedPerSample, SamplesPerPixel>;
            case ::gdcm::PixelFormat::ScalarType::UINT32:
                return transform_convert<TargetType, uint32_t, BitsAllocatedPerSample, SamplesPerPixel>;
            case ::gdcm::PixelFormat::ScalarType::UINT64:
                return transform_convert<TargetType, uint64_t, BitsAllocatedPerSample, SamplesPerPixel>;
            case ::gdcm::PixelFormat::ScalarType::SINGLEBIT:
                return transform_convert<TargetType, void, BitsAllocatedPerSample, SamplesPerPixel>;
            }

            ec = {}; // error unhandled/unknown scalar type
            return nullptr;
        }

        template<typename TargetType, size_t BitsAllocatedPerSample>
        inline transformation_f_t<TargetType> transformation_f_selector(const ::gdcm::PixelFormat& pixelFormat, error_code& ec)
        {
            auto samplesPerPixel = pixelFormat.GetSamplesPerPixel();

            switch (samplesPerPixel)
            {
            case 1:
                return transformation_f_selector<TargetType, BitsAllocatedPerSample, 1>(pixelFormat, ec);
            case 3:
                return transformation_f_selector<TargetType, BitsAllocatedPerSample, 3>(pixelFormat, ec);
            case 4:
                return transformation_f_selector<TargetType, BitsAllocatedPerSample, 4>(pixelFormat, ec);
            }

            ec = {}; // error invalid samples per pixel count
            return nullptr;
        }

        template<typename TargetType>
        inline transformation_f_t<TargetType> transformation_f_selector(const ::gdcm::PixelFormat& pixelFormat, error_code& ec)
        {
            auto bitsPerSample = pixelFormat.GetBitsAllocated();

            switch (bitsPerSample)
            {
            case sizeof(uint8_t) * 8:
                return transformation_f_selector<TargetType, sizeof(uint8_t) * 8>(pixelFormat, ec);
            case sizeof(uint16_t) * 8:
                return transformation_f_selector<TargetType, sizeof(uint16_t) * 8>(pixelFormat, ec);
            case sizeof(uint32_t) * 8:
                return transformation_f_selector<TargetType, sizeof(uint32_t) * 8>(pixelFormat, ec);
            case sizeof(uint64_t) * 8:
                return transformation_f_selector<TargetType, sizeof(uint64_t) * 8>(pixelFormat, ec);
            }

            ec = {}; // error invalid bits per sample count
            return nullptr;
        }
    }

    template<typename TargetType>
    inline error_code transform_convert(core::view<TargetType> destination, core::const_view<std::byte> source, size_t element_count, const ::gdcm::PixelFormat& pixelFormat, size_t rgba_plane_index = 0)
    {
        error_code ec;
        auto transform_convert_f = detail::transformation_f_selector<TargetType>(pixelFormat, ec);
        if (transform_convert_f == nullptr)
            return ec;

        transform_convert_f(destination, source, element_count, rgba_plane_index);

        return {};
    }
}