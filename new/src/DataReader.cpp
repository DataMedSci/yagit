/******************************************************************************************
 * This file is part of 'yet Another Gamma Index Tool'.
 *
 * 'yet Another Gamma Index Tool' is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * 'yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 'yet Another Gamma Index Tool'; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 ******************************************************************************************/

#include "DataReader.hpp"

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

std::vector<uint8_t> getPixelData(const gdcm::DataSet& ds){
    if(!ds.FindDataElement(PixelDataTag)){
        return {};
    }
    const gdcm::DataElement& data = ds.GetDataElement(PixelDataTag);
    uint32_t dataLength = data.GetVL();
    const char* dataPtr = data.GetByteValue()->GetPointer();
    return std::vector<uint8_t>(dataPtr, dataPtr + dataLength);
}

template<typename T>
void swapBytesToSystemEndianness(std::vector<uint8_t>& data, gdcm::SwapCode dataEndianness){
    gdcm::ByteSwap<T>::SwapRangeFromSwapCodeIntoSystem(reinterpret_cast<T*>(data.data()), dataEndianness, data.size());
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

ImageData readRTDoseDicom(const std::string& file, bool displayInfo){
    gdcm::Reader reader;
    reader.SetFileName(file.c_str());
    if(!reader.Read()) {
        throw std::runtime_error("cannot read " + file + " file");
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
            if(gridFrameOffsetVector.size() != *frames){
                throw std::runtime_error("DICOM file doesn't have attribute Slice Thickness (0018,0050) and Grid Frame Offset Vector (3004,000C) with " + std::to_string(*frames) + " elements");
            }
            sliceThicknessVal = gridFrameOffsetVector[1] - gridFrameOffsetVector[0];
            for(int i=2; i < gridFrameOffsetVector.size(); i++){
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

    std::vector<uint8_t> pixelData = getPixelData(ds);
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
        const uint32_t* dataPtr = reinterpret_cast<uint32_t*>(pixelData.data());
        for(int i=0; i < doseDataSize; i++, dataPtr++){
            doseData.emplace_back(static_cast<float>(static_cast<double>(*dataPtr) * doseGridScaling.value()));
        }
    }
    else if(*bitsAllocated == 16){
        const uint16_t* dataPtr = reinterpret_cast<uint16_t*>(pixelData.data());
        for(int i=0; i < doseDataSize; i++, dataPtr++){
            doseData.emplace_back(static_cast<float>(static_cast<double>(*dataPtr) * doseGridScaling.value()));
        }
    }

    DataSize size(*frames, *rows, *columns);
    DataOffset offset(static_cast<float>(imagePositionPatient[2]),
                      static_cast<float>(imagePositionPatient[1]),
                      static_cast<float>(imagePositionPatient[0]));
    DataSpacing spacing(static_cast<float>(sliceThicknessVal),
                        static_cast<float>(pixelSpacing[0]),
                        static_cast<float>(pixelSpacing[1]));

    return ImageData(std::move(doseData), size, offset, spacing);
}

}
