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

#ifndef GIYAGIT_DICOM_TAGS_H
#define GIYAGIT_DICOM_TAGS_H

// [CS] type of equipment used to acquire data
#define DCM_MODALITY 0x0008,0x0060
// [DS] distance between centers of slices in mm
#define DCM_SPACING_BETWEEN_SLICES 0x0018,0x0088
// [DS] x, y, z coordinates of the center of the first voxel
#define DCM_IMAGE_POSITION 0x0020,0x0032
// [DS] row, column distance in mm between centers of pixels
#define DCM_PIXEL_SPACING 0x0028,0x0030
// [DS] b and m respectively in equation output = m * (value in pixel data) + b
#define DCM_RESCALE_INTERCEPT 0x0028,0x1052
#define DCM_RESCALE_SLOPE 0x0028,0x1053
// [DS] vector of offsets between planes in multi-frame doses in mm
#define DCM_GRID_FRAME_OFFSET_VECTOR 0x3004,0x000C
// [DS] factor that when multiplied by values in pixel data yields grid dose in specified dose units
#define DCM_DOSE_GRID_SCALING 0x3004,0x000E
// [OB|OW] pixel data when in bytes or words (integers)
#define PIXEL_DATA 0x7FE0,0010

// tag group with various pixel data representations
#define DCM_GROUP_PIXEL_DATA 0x7FE0

#endif
