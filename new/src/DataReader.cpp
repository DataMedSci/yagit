/********************************************************************************************
 * Copyright (C) 2023 'yet Another Gamma Index Tool' Developers.
 * 
 * This file is part of 'yet Another Gamma Index Tool'.
 * 
 * 'yet Another Gamma Index Tool' is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * 'yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 'yet Another Gamma Index Tool'.  If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************************************/

#include "DataReader.hpp"

#include <fstream>
#include <optional>

#include <gdcmReader.h>
#include <gdcmAttribute.h>
#include <gdcmTagKeywords.h>
#include <gdcmByteSwap.h>

namespace yagit::DataReader{

namespace{
const gdcm::Keywords::TransferSyntaxUID TransferSyntaxUIDAttr{};                   // (0x0002, 0x0010)  UI
const gdcm::Keywords::Modality ModalityAttr{};                                     // (0x0008, 0x0060)  CS
const gdcm::Keywords::SliceThickness SliceThicknessAttr{};                         // (0x0018, 0x0050)  DS
const gdcm::Keywords::ImagePositionPatient ImagePositionPatientAttr{};             // (0x0020, 0x0032)  DS
const gdcm::Keywords::ImageOrientationPatient ImageOrientationPatientAttr{};       // (0x0020, 0x0037)  DS
const gdcm::Keywords::SamplesPerPixel SamplesPerPixelAttr{};                       // (0x0028, 0x0002)  US
const gdcm::Keywords::PhotometricInterpretation PhotometricInterpretationAttr{};   // (0x0028, 0x0004)  CS
const gdcm::Keywords::NumberOfFrames NumberOfFramesAttr{};                         // (0x0028, 0x0008)  IS
const gdcm::Keywords::Rows RowsAttr{};                                             // (0x0028, 0x0010)  US
const gdcm::Keywords::Columns ColumnsAttr{};                                       // (0x0028, 0x0011)  US
const gdcm::Keywords::PixelSpacing PixelSpacingAttr{};                             // (0x0028, 0x0030)  DS
const gdcm::Keywords::BitsAllocated BitsAllocatedAttr{};                           // (0x0028, 0x0100)  US
const gdcm::Keywords::BitsStored BitsStoredAttr{};                                 // (0x0028, 0x0101)  US
const gdcm::Keywords::HighBit HighBitAttr{};                                       // (0x0028, 0x0102)  US
const gdcm::Keywords::PixelRepresentation PixelRepresentationAttr{};               // (0x0028, 0x0103)  US
const gdcm::Keywords::GridFrameOffsetVector GridFrameOffsetVectorAttr;             // (0x3004, 0x000C)  DS
const gdcm::Keywords::DoseGridScaling DoseGridScalingAttr{};                       // (0x3004, 0x000E)  DS

const gdcm::Tag PixelDataTag{0x7FE0, 0x0010};

const gdcm::UIComp ImplicitVRLittleEndian{"1.2.840.10008.1.2"};
const gdcm::UIComp ExplicitVRLittleEndian{"1.2.840.10008.1.2.1"};
const gdcm::UIComp DeflatedExplicitVRLittleEndian{"1.2.840.10008.1.2.1.99"};
const gdcm::UIComp ExplicitVRBigEndian{"1.2.840.10008.1.2.2"};

const gdcm::CSComp RTDoseModality{"RTDOSE"};
const gdcm::CSComp Monochrome2{"MONOCHROME2"};
}

namespace{
template<uint16_t G, uint16_t E>
std::optional<typename gdcm::Attribute<G, E>::ArrayType> getValue(const gdcm::DataSet& ds, gdcm::Attribute<G, E> attr){
    if(!ds.FindDataElement(attr.GetTag())){
        return std::nullopt;
    }
    attr.Set(ds);
    return std::optional{attr.GetValue()};
}

template<uint16_t G, uint16_t E>
std::vector<typename gdcm::Attribute<G, E>::ArrayType> getMultipleValues(const gdcm::DataSet& ds, gdcm::Attribute<G, E> attr){
    if(!ds.FindDataElement(attr.GetTag())){
        return {};
    }
    attr.Set(ds);
    return {&attr.GetValue(0), &attr.GetValue(attr.GetNumberOfValues()-1) + 1};
}

std::vector<char> getPixelData(const gdcm::DataSet& ds){
    if(!ds.FindDataElement(PixelDataTag)){
        return {};
    }
    const gdcm::DataElement& data = ds.GetDataElement(PixelDataTag);
    uint32_t dataLength = data.GetVL();
    const char* dataPtr = data.GetByteValue()->GetPointer();
    return std::vector<char>(dataPtr, dataPtr + dataLength);
}

template<typename T>
void swapBytesToSystemEndianness(std::vector<char>& data, gdcm::SwapCode dataEndianness){
    gdcm::ByteSwap<T>::SwapRangeFromSwapCodeIntoSystem(reinterpret_cast<T*>(data.data()), dataEndianness, data.size() / sizeof(T));
}

gdcm::SwapCode getDataEndianness(const std::optional<gdcm::UIComp>& transferSyntaxUID){
    if(!transferSyntaxUID.has_value() || 
       *transferSyntaxUID == ImplicitVRLittleEndian || 
       *transferSyntaxUID == ExplicitVRLittleEndian || 
       *transferSyntaxUID == DeflatedExplicitVRLittleEndian){
        return gdcm::SwapCode::LittleEndian;
    }
    else if(*transferSyntaxUID == ExplicitVRBigEndian){
        return gdcm::SwapCode::BigEndian;
    }
    else{
        return gdcm::SwapCode::Unknown;
    }
}
}

ImageData readRTDoseDicom(const std::string& filepath, bool displayInfo){
    gdcm::Reader reader;
    reader.SetFileName(filepath.c_str());
    if(!reader.Read()) {
        throw std::runtime_error("cannot read " + filepath + " file");
    }
    
    const gdcm::DataSet& ds = reader.GetFile().GetDataSet();

    auto modality = getValue(ds, ModalityAttr);
    if(modality == std::nullopt || *modality != RTDoseModality){
        throw std::runtime_error("DICOM file doesn't have attribute Modality (0008,0060) equal to '" + RTDoseModality + "'");
    }

    auto frames = getValue(ds, NumberOfFramesAttr);
    if(frames == std::nullopt){
        throw std::runtime_error("DICOM file doesn't have attribute Number of Frames (0028,0008)");
    }
    auto rows = getValue(ds, RowsAttr);
    if(rows == std::nullopt){
        throw std::runtime_error("DICOM file doesn't have attribute Rows (0028,0010)");
    }
    auto columns = getValue(ds, ColumnsAttr);
    if(columns == std::nullopt){
        throw std::runtime_error("DICOM file doesn't have attribute Columns (0028,0011)");
    }

    auto imagePositionPatient = getMultipleValues(ds, ImagePositionPatientAttr);  // xyz coordinates
    if(imagePositionPatient.size() != 3){
        throw std::runtime_error("DICOM file doesn't have attribute Image Position Patient (0020,0032)");
    }

    auto imageOrientationPatient = getMultipleValues(ds, ImageOrientationPatientAttr);
    if(imageOrientationPatient.size() != 6){
        throw std::runtime_error("DICOM file doesn't have attribute Image Orientation Patient (0020,0037) containig 6 elements");
    }
    // direction cosines sometimes have innacurate values (e.g. -2.05203471e-10 instead of 0), so we round them
    for(auto& el : imageOrientationPatient){
        el = static_cast<int>(el * 1e5 + 0.5) / 1e5;  // round number to 5 decimal places
    }
    // TODO: check if abs_orient is [1,0,0,0,1,0] or [0,1,0,1,0,0] - other are not supported
    // TODO: add convertion of spacing depending on orientation??

    auto pixelSpacing = getMultipleValues(ds, PixelSpacingAttr);  // row and column spacing
    if(pixelSpacing.size() != 2){
        throw std::runtime_error("DICOM file doesn't have attribute Pixel Spacing (0028,0030) containig 2 elements");
    }

    double sliceThicknessVal{0};
    if(*frames > 1){
        auto sliceThickness = getValue(ds, SliceThicknessAttr);
        if(sliceThickness == std::nullopt || *sliceThickness == 0){
            auto gridFrameOffsetVector = getMultipleValues(ds, GridFrameOffsetVectorAttr);
            if(gridFrameOffsetVector.size() != static_cast<size_t>(*frames)){
                throw std::runtime_error("DICOM file doesn't have attribute Slice Thickness (0018,0050) and Grid Frame Offset Vector (3004,000C) with " + std::to_string(*frames) + " elements");
            }
            sliceThicknessVal = gridFrameOffsetVector[1] - gridFrameOffsetVector[0];
            for(size_t i=2; i < gridFrameOffsetVector.size(); i++){
                if(gridFrameOffsetVector[i] - gridFrameOffsetVector[i-1] != sliceThicknessVal){
                    throw std::runtime_error("uneven spacing in Grid Frame Offset Vector (3004,000C) not supported");
                }
            }
        }
        else{
            sliceThicknessVal = *sliceThickness;
        }
    }

    auto doseGridScaling = getValue(ds, DoseGridScalingAttr);
    if(doseGridScaling == std::nullopt){
        throw std::runtime_error("DICOM file doesn't have attribute Dose Grid Scaling (3004,000E)");
    }

    auto samplesPerPixel = getValue(ds, SamplesPerPixelAttr);
    if(samplesPerPixel == std::nullopt || *samplesPerPixel != 1){
        throw std::runtime_error("DICOM file doesn't have attribute Samples Per Pixel (0028,0002) equal to 1");
    }
    auto photometricInterpretation = getValue(ds, PhotometricInterpretationAttr);
    if(photometricInterpretation == std::nullopt || *photometricInterpretation != Monochrome2){
        throw std::runtime_error("DICOM file doesn't have attribute Photometric Interpretation (0028,0004) equal to '" + Monochrome2 + "'");
    }
    auto bitsAllocated = getValue(ds, BitsAllocatedAttr);
    if(bitsAllocated == std::nullopt || (*bitsAllocated != 16 && *bitsAllocated != 32)){
        throw std::runtime_error("DICOM file doesn't have attribute Bits Allocated (0028,0100) equal to 16 or 32");
    }
    auto bitsStored = getValue(ds, BitsStoredAttr);
    if(bitsStored == std::nullopt || (*bitsStored != *bitsAllocated)){
        throw std::runtime_error("DICOM file doesn't have attribute Bits Stored (0028,0101) equal to attribute Bits Allocated");
    }
    auto highBit = getValue(ds, HighBitAttr);
    if(highBit == std::nullopt || (*highBit != *bitsAllocated - 1)){
        throw std::runtime_error("DICOM file doesn't have attribute High Bit (0028,0102) equal to attribute Bits Allocated - 1");
    }
    auto pixelRepresentation = getValue(ds, PixelRepresentationAttr);  // 0 - unsigned, 1 - signed
    if(pixelRepresentation == std::nullopt || (*pixelRepresentation != 0)){
        throw std::runtime_error("DICOM file doesn't have attribute Pixel Representation (0028,0103) equal to 0");
    }

    std::vector<char> pixelData = getPixelData(ds);
    uint32_t correctDataSize = frames.value() * rows.value() * columns.value() * bitsAllocated.value() / 8; // expected number of bytes of data
    if(pixelData.size() != correctDataSize){
        throw std::runtime_error("DICOM file doesn't have attribute Pixel Data (7FE0,0010) containing " + std::to_string(correctDataSize) + " bytes of data");
    }

    auto transferSyntaxUID = getValue(ds, TransferSyntaxUIDAttr);
    gdcm::SwapCode dataEndianness = getDataEndianness(transferSyntaxUID);
    if(dataEndianness != gdcm::SwapCode::Unknown){
        if(*bitsAllocated == 32){
            swapBytesToSystemEndianness<uint32_t>(pixelData, dataEndianness);
        }
        else if(*bitsAllocated == 16){
            swapBytesToSystemEndianness<uint16_t>(pixelData, dataEndianness);
        }
    }
    else{
        throw std::runtime_error("compression Transfer Syntax UIDs not supported");
    }

    if(displayInfo){
        std::cout << "Modality: " << *modality << "\n";
        std::cout << "TransferSyntaxUID: " << (transferSyntaxUID.has_value() ? std::string(*transferSyntaxUID)
                                                                             : std::string("")) << "\n";
        std::cout << "NumberOfFrames: " << *frames << "\n"
                  << "Rows: " << *rows << "\n"
                  << "Columns: " << *columns << "\n"
                  << "ImagePositionPatient: " << imagePositionPatient[0] << " "   // x
                                              << imagePositionPatient[1] << " "   // y
                                              << imagePositionPatient[2] << "\n"  // z
                  << "ImageOrientationPatient: " << imageOrientationPatient[0] << " "
                                                 << imageOrientationPatient[1] << " "
                                                 << imageOrientationPatient[2] << " "
                                                 << imageOrientationPatient[3] << " "
                                                 << imageOrientationPatient[4] << " "
                                                 << imageOrientationPatient[5] << "\n";
        std::cout << "PixelSpacing: " << pixelSpacing[0] << " "   // row spacing
                                      << pixelSpacing[1] << "\n"  // column spacing
                  << "SliceThickness value: " << sliceThicknessVal << "\n";  // frame spacing
        std::cout << "DoseGridScaling: " << *doseGridScaling << "\n";
        std::cout << "SamplesPerPixel: " << *samplesPerPixel << "\n"
                  << "PhotometricInterpretation: " << *photometricInterpretation << "\n"
                  << "BitsAllocated: " << *bitsAllocated << "\n"
                  << "BitsStored: " << *bitsStored << "\n"
                  << "HighBit: " << *highBit << "\n"
                  << "PixelRepresentation: " << *pixelRepresentation << "\n";
        std::cout << "PixelData: array of " << pixelData.size() << " bytes\n";
    }


    std::vector<float> doseData;
    const size_t doseDataSize = pixelData.size() / (*bitsAllocated / 8);  // number of elements
    doseData.reserve(doseDataSize);

    // convert uint pixelData to float doseData
    // dose = float(double(PixelData) * double(DoseGridScaling))
    // Dose is float, because there is little difference between double and float doses.
    // And in gamma calculations, this difference has little significance.
    // Thanks to this, the data takes up less memory and calculations are performed faster.
    if(*bitsAllocated == 32){
        const uint32_t* dataPtr = reinterpret_cast<const uint32_t*>(pixelData.data());
        for(size_t i=0; i < doseDataSize; i++, dataPtr++){
            doseData.emplace_back(static_cast<float>(static_cast<double>(*dataPtr) * doseGridScaling.value()));
        }
    }
    else if(*bitsAllocated == 16){
        const uint16_t* dataPtr = reinterpret_cast<const uint16_t*>(pixelData.data());
        for(size_t i=0; i < doseDataSize; i++, dataPtr++){
            doseData.emplace_back(static_cast<float>(static_cast<double>(*dataPtr) * doseGridScaling.value()));
        }
    }

    DataSize size{static_cast<uint32_t>(*frames),
                  static_cast<uint32_t>(*rows),
                  static_cast<uint32_t>(*columns)};
    DataOffset offset{static_cast<float>(imagePositionPatient[2]),
                      static_cast<float>(imagePositionPatient[1]),
                      static_cast<float>(imagePositionPatient[0])};
    DataSpacing spacing{static_cast<float>(sliceThicknessVal),
                        static_cast<float>(pixelSpacing[0]),
                        static_cast<float>(pixelSpacing[1])};

    return ImageData(std::move(doseData), size, offset, spacing);
}

namespace{
const std::string ObjectTypeTag{"ObjectType"};
const std::string NDimsTag{"NDims"};
const std::string DimSizeTag{"DimSize"};
const std::string OffsetTag{"Offset"};
const std::string PositionTag{"Position"};
const std::string OriginTag{"Origin"};
const std::string ElementSpacingTag{"ElementSpacing"};
const std::string ElementSizeTag{"ElementSize"};
const std::string RotationTag{"Rotation"};
const std::string OrientationTag{"Orientation"};
const std::string TransformMatrixTag{"TransformMatrix"};
const std::string BinaryDataTag{"BinaryData"};
const std::string BinaryDataByteOrderMSBTag{"BinaryDataByteOrderMSB"};
const std::string CompressedDataTag{"CompressedData"};
const std::string ElementTypeTag{"ElementType"};
const std::string ElementDataFileTag{"ElementDataFile"};

const std::string AsciiCharType{"MET_ASCII_CHAR_TYPE"};
const std::string CharType{"MET_CHAR"};
const std::string UcharType{"MET_UCHAR"};
const std::string ShortType{"MET_SHORT"};
const std::string UshortType{"MET_USHORT"};
const std::string IntType{"MET_INT"};
const std::string UintType{"MET_UINT"};
const std::string LongType{"MET_LONG"};
const std::string UlongType{"MET_ULONG"};
const std::string LongLongType{"MET_LONG_LONG"};
const std::string UlongLongType{"MET_ULONG_LONG"};
const std::string FloatType{"MET_FLOAT"};
const std::string DoubleType{"MET_DOUBLE"};
}

namespace{
template <typename T>
void convertPixelDataToFloatData(const std::vector<char>& pixelData, std::vector<float>& floatData){
    const T* dataPtr = reinterpret_cast<const T*>(pixelData.data());
    const size_t dataSize = pixelData.size() / sizeof(T);
    for(size_t i=0; i < dataSize; i++, dataPtr++){
        floatData.emplace_back(static_cast<float>(*dataPtr));
    }
}

std::string ltrim(std::string str){
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char c) {
        return !std::isspace(c);
    }));
    return str;
}
std::string rtrim(std::string str){
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char c) {
        return !std::isspace(c);
    }).base(), str.end());
    return str;
}
std::string trim(std::string str){
    return ltrim(rtrim(str));
}

inline void swapBytes64(uint64_t& value){
    value = (((value & 0xff00000000000000ull) >> 56) |
             ((value & 0x00ff000000000000ull) >> 40) |
             ((value & 0x0000ff0000000000ull) >> 24) |
             ((value & 0x000000ff00000000ull) >> 8 ) |
             ((value & 0x00000000ff000000ull) << 8 ) |
             ((value & 0x0000000000ff0000ull) << 24) |
             ((value & 0x000000000000ff00ull) << 40) |
             ((value & 0x00000000000000ffull) << 56));
}

void swapBytes64(std::vector<char>& data){
    uint64_t* dataPtr = reinterpret_cast<uint64_t*>(data.data());
    for(size_t i = 0; i < data.size() / sizeof(uint64_t); i++, dataPtr++){
        swapBytes64(*dataPtr);
    }
}
}

ImageData readMetaImage(const std::string& filepath, bool displayInfo){
    std::ifstream file(filepath, std::ios::binary);
    if(!file.is_open()){
        throw std::runtime_error("cannot read " + filepath + " file");
    }

    DataSize size{};
    DataOffset offset{};
    DataSpacing spacing{};

    gdcm::SwapCode dataEndianness = gdcm::SwapCode::LittleEndian;
    uint32_t typeBytesSize = 1;
    std::string type{""};

    bool objectTypeOccurred = false;
    bool nDimsOccurred = false;
    bool dimSizeOccurred = false;
    bool offsetOccurred = false;
    bool elementSpacingOccurred = false;
    bool elementTypeOccurred = false;
    bool elementDataFileOccurred = false;

    std::string line;
    while(std::getline(file, line)){
        size_t eqPos = line.find('=');
        const std::string tag = trim(line.substr(0, eqPos));
        const std::string value = trim(line.substr(eqPos+1));

        if(displayInfo){
            std::cout << tag << " = " << value << "\n";
        }

        if(tag == ObjectTypeTag){
            if(value != "Image"){
                throw std::runtime_error("MetaImage file doesn't have value Image in ObjectType tag");
            }
            objectTypeOccurred = true;
        }
        else if(tag == NDimsTag){
            // TODO: add support for 2D images
            if(value != "3"){
                throw std::runtime_error("non 3-dimensional data not supported");
            }
            nDimsOccurred = true;
        }
        else if(tag == DimSizeTag){
            std::istringstream ss(value);
            ss >> size.columns >> size.rows >> size.frames;
            if(ss.fail()){
                throw std::runtime_error(tag + " tag has too few elements or some elements aren't integers");
            }
            dimSizeOccurred = true;
        }
        else if(tag == OffsetTag || tag == PositionTag || tag == OriginTag){
            std::istringstream ss(value);
            ss >> offset.columns >> offset.rows >> offset.frames;
            if(ss.fail()){
                throw std::runtime_error(tag + " tag has too few elements or some elements aren't floats");
            }
            offsetOccurred = true;
        }
        else if(tag == ElementSpacingTag || tag == ElementSizeTag){
            std::istringstream ss(value);
            ss >> spacing.columns >> spacing.rows >> spacing.frames;
            if(ss.fail()){
                throw std::runtime_error(tag + " tag has too few elements or some elements aren't floats");
            }
            elementSpacingOccurred = true;
        }
        else if(tag == RotationTag || tag == OrientationTag || tag == TransformMatrixTag){
            int orient[9];
            std::istringstream ss(value);
            ss >> orient[0] >> orient[1] >> orient[2]
               >> orient[3] >> orient[4] >> orient[5]
               >> orient[6] >> orient[7] >> orient[8];
            // TODO: add support for different orientations
            if(ss.fail()){
                throw std::runtime_error(tag + " tag has too few elements or some elements aren't integers");
            }
        }
        else if(tag == BinaryDataTag){
            if(value != "True"){
                throw std::runtime_error("non binary data not supported");
            }
        }
        else if(tag == BinaryDataByteOrderMSBTag){
            dataEndianness = (value == "True" ? gdcm::SwapCode::BigEndian : gdcm::SwapCode::LittleEndian);
        }
        else if(tag == CompressedDataTag){
            if(value != "False"){
                throw std::runtime_error("compressed data not supported");
            }
        }
        else if(tag == ElementTypeTag){
            if(value == AsciiCharType || value == CharType || value == UcharType){
                typeBytesSize = 1;
            }
            else if(value == ShortType || value == UshortType){
                typeBytesSize = 2;
            }
            else if(value == IntType || value == UintType ||
                    value == LongType || value == UlongType ||
                    value == FloatType){
                typeBytesSize = 4;
            }
            else if(value == LongLongType || value == UlongLongType ||
                    value == DoubleType){
                typeBytesSize = 8;
            }
            else{
                throw std::runtime_error(value + " data type not supported");
            }
            type = value;
            elementTypeOccurred = true;
        }
        else if(tag == ElementDataFileTag){
            if(value != "LOCAL"){
                throw std::runtime_error("non-local data not supported");
            }
            elementDataFileOccurred = true;
            break;
        }
    }

    // check if all necessary tags occurred
    if(objectTypeOccurred == false){
        throw std::runtime_error("ObjectType tag didn't occurred");
    }
    if(nDimsOccurred == false){
        throw std::runtime_error("NDims tag didn't occurred");
    }
    if(dimSizeOccurred == false){
        throw std::runtime_error("DimSize tag didn't occurred");
    }
    if(offsetOccurred == false){
        throw std::runtime_error("Offset/Position/Origin tag didn't occurred");
    }
    if(elementSpacingOccurred == false){
        throw std::runtime_error("ElementSpacing/ElementSize tag didn't occurred");
    }
    if(elementTypeOccurred == false){
        throw std::runtime_error("ElementType tag didn't occurred");
    }
    if(elementDataFileOccurred == false){
        throw std::runtime_error("ElementDataFile tag didn't occurred");
    }

    // read pixel data
    size_t dataSize = size.frames * size.rows * size.columns;
    size_t bytes = dataSize * typeBytesSize;
    std::vector<char> pixelData(bytes);
    file.read(pixelData.data(), bytes);
    std::streamsize bytesRead = file.gcount();
    if(bytesRead != bytes){
        throw std::runtime_error("pixel data doesn't contain " + std::to_string(bytes) + " bytes of data");
    }

    file.close();

    // convert data to system endianness
    gdcm::SwapCode systemEndianness = (gdcm::ByteSwap<uint16_t>::SystemIsBigEndian() ? gdcm::SwapCode::BigEndian
                                                                                     : gdcm::SwapCode::LittleEndian);
    if(systemEndianness != dataEndianness){
        if(typeBytesSize == 2){
            swapBytesToSystemEndianness<uint16_t>(pixelData, dataEndianness);
        }
        else if(typeBytesSize == 4){
            swapBytesToSystemEndianness<uint32_t>(pixelData, dataEndianness);
        }
        // gdcm doesn't support bytes swap for 64-bit data, so we use our own
        else if(typeBytesSize == 8){
            swapBytes64(pixelData);
        }
    }

    // convert bytes data to float data
    std::vector<float> floatData;
    floatData.reserve(dataSize);

    if(type == AsciiCharType || type == CharType){
        convertPixelDataToFloatData<int8_t>(pixelData, floatData);
    }
    else if(type == UcharType){
        convertPixelDataToFloatData<uint8_t>(pixelData, floatData);
    }
    else if(type == ShortType){
        convertPixelDataToFloatData<int16_t>(pixelData, floatData);
    }
    else if(type == UshortType){
        convertPixelDataToFloatData<uint16_t>(pixelData, floatData);
    }
    else if(type == IntType || type == LongType){
        convertPixelDataToFloatData<int32_t>(pixelData, floatData);
    }
    else if(type == UintType || type == UlongType){
        convertPixelDataToFloatData<uint32_t>(pixelData, floatData);
    }
    else if(type == LongLongType){
        convertPixelDataToFloatData<int64_t>(pixelData, floatData);
    }
    else if(type == UlongLongType){
        convertPixelDataToFloatData<uint64_t>(pixelData, floatData);
    }
    else if(type == FloatType){
        convertPixelDataToFloatData<float>(pixelData, floatData);
    }
    else if(type == DoubleType){
        convertPixelDataToFloatData<double>(pixelData, floatData);
    }

    return ImageData(std::move(floatData), size, offset, spacing);
}

}
