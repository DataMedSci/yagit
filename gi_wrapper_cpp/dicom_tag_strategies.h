/** @file */

/*********************************************************************************************************************
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
 *********************************************************************************************************************/

#ifndef DICOM_TAG_STATEGIES_H
#define DICOM_TAG_STATEGIES_H

#include <set>


namespace gdcm {
    class DataSet;
}

typedef bool (*tagsRewriteStrategy)(int, int, std::set<std::pair<int, int>>);



/// \brief Function copies DICOM tags from source DICOM into result DICOM.
///
/// \note Example of how we can define tagSet parameter:
///
///       tagSet =  {
///           {0x0020, 0x0030},    // DICOM ImagePosition Tag
///              ^       ^-----------------TagID
///              '----------------GroupId
///           {0x0010, 0x0010},    // DICOM PatientName Tag
///       };
///
/// \param source                Object in which GDCM library stores parsed DICOM file from which the tags are rewritten.
///                              The pointer returned by loadDicom() function.
/// \param destination           Object in which GDCM library stores parsed DICOM file to which the tags are rewritten.
///                              The pointer returned by loadDicom() function.
/// \param rewriteTagsStrategyId Specifies the strategy of transferring tags from reference DICOM to output DICOM file:
///                               - 5 - rewrite all tags, except these specified in tags parameter
///                               - 4 - rewrite only these specified in tags parameter
///                               - 3 - rewrite only not private tags
///                               - 2 - rewrite no tags
///                               - 1 or other integer values - rewrite all tags
/// \param userTags               Set that contains representation of DICOM tags. Used in strategy 4 and 5.
///
/// \return Pointer to the image matrix. Its size can be calculated using parameters: dims, xNumber, yNumber and zNumber.
///
///////////////////////////////////////////////////////////////////////////////
void applyDicomTagsRewriteStrategy(gdcm::DataSet& source, gdcm::DataSet& destination,
    int rewriteTagsStrategyId, std::set<std::pair<int, int>> userTags);

#endif