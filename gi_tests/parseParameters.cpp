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

#include "parseParameters.h"
using namespace std;

Parameters* ParseParameters::parseGIParametersFromFile(string path)
{
  Parameters* parms = new Parameters();
  parms->noDimentions = 2.5;

  ifstream paramsFile;
  paramsFile.open(path.c_str());

  if(paramsFile.is_open())
  {
    string boofor;

    getline(paramsFile, boofor);
    parms->noDimentions = getIntAttribute(boofor, "noDimentions");

    getline(paramsFile, boofor);
    parms->perc = getDoubleAttribute(boofor, "perc");

    getline(paramsFile, boofor);
    parms->dta = getDoubleAttribute(boofor, "dta");

    getline(paramsFile, boofor);
    parms->local = getIsLocalAttribute(boofor, "local");

    getline(paramsFile, boofor);
    parms->refval = getDoubleAttribute(boofor, "refval");

    getline(paramsFile, boofor);
    parms->limit = getDoubleAttribute(boofor, "limit");

    getline(paramsFile, boofor);
    parms->plane = getIntAttribute(boofor, "plane");

    getline(paramsFile, boofor);
    parms->refslice = getIntAttribute(boofor, "refslice");

    getline(paramsFile, boofor);
    parms->tarslice = getIntAttribute(boofor, "tarslice");

    getline(paramsFile, boofor);
    parms->cmpObj = getComparator(boofor, "comparator");

    getline(paramsFile, boofor);
    parms->epsilon = getDoubleAttribute(boofor, "epsilon");
  }
  paramsFile.close();
  return parms;
}
