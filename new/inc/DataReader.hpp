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
#pragma once

#include <string>

#include "DoseData.hpp"

namespace yagit::DataReader{

/**
 * @brief Read RT Dose DICOM file and retrieve dose image from it
 * @param file File path to RT Dose DICOM (.dcm)
 * @param displayInfo Whether to show additional information read from the file
 * @return Image containing dose and image info (size, offset, spacing) retrieved from DICOM file
 */
DoseData readRTDoseDicom(const std::string& file, bool displayInfo = false);

// TODO: other methods for reading other data (e.g. MetaImage)

}
