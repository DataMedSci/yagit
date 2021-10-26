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

#include "gdcmReader.h"
#include "gdcmGlobal.h"
#include "gdcmDicts.h"
#include "gdcmDict.h"
#include "gdcmAttribute.h"
#include "gdcmStringFilter.h"
#include "gdcmImageReader.h"
#include "gdcmImageWriter.h"

using namespace std;
using namespace gdcm;

class Comparison
{
public:
  Comparison(int dims_, gdcm::File& refFile_, double* gamma_,
    int refXNumber_, int refYNumber_, int refZNumber_, int rescale_, string info_, int size_, double limit_) :
    dims(dims_), refFile(refFile_), gamma(gamma_), refXNumber(refXNumber_),
    refYNumber(refYNumber_), refZNumber(refZNumber_), rescale(rescale_), info(info_), size(size_), limit(limit_){}
  int dims;
  gdcm::File& refFile;
  double* gamma;
  int refXNumber;
  int refYNumber;
  int refZNumber;
  int rescale;
  string info;
  int size;
  double limit;
};
