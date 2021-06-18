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

#include "ParametersParser.h"

Parameters* ParametersParser::resolveParameters(wxString dims, wxString dd, wxString dta, wxString refVal,
  wxString local, wxString limit, wxString rescale, wxString plane, wxString refSlice, wxString tarSlice,
  wxString refA, wxString refB, wxString tarA, wxString tarB)
{
  int parsedDims = parseDims(dims.ToStdString());
  double parsedDD = stod(dd.ToStdString());
  double parsedDTA = stod(dta.ToStdString());

  double parsedRefVal = -1;
  if(refVal.ToStdString() != "")
    parsedRefVal = stod(refVal.ToStdString());

  bool parsedLocal = parseLocal(local.ToStdString());
  double parsedLimit = stod(limit.ToStdString());
  int parsedRescale = stoi(rescale.ToStdString());

  int parsedPlane = -1;
  if(plane.ToStdString() == "XY")
    parsedPlane = 0;
  else if(plane.ToStdString() == "XZ")
    parsedPlane = 1;
  else if(plane.ToStdString() == "YZ")
    parsedPlane = 2;

  int parsedRefSlice = -1;
  if(refSlice.ToStdString() != "")
    parsedRefSlice= stoi(refSlice.ToStdString());

  int parsedTarSlice = -1;
  if(tarSlice.ToStdString() != "")
    parsedTarSlice = stoi(tarSlice.ToStdString());

  double parsedRefA = 1;
  if(refA.ToStdString() != "")
    parsedRefA = stod(refA.ToStdString());

  double parsedRefB = 0;
  if(refB.ToStdString() != "")
    parsedRefB = stod(refB.ToStdString());

  double parsedTarA = 1;
  if(tarA.ToStdString() != "")
    parsedTarA = stod(tarA.ToStdString());

  double parsedTarB = 0;
  if(tarB.ToStdString() != "")
    parsedTarB = stod(tarB.ToStdString());

  Parameters* parameters = new Parameters(parsedDims, parsedDD, parsedDTA, parsedRefVal, parsedLocal, parsedLimit, parsedRescale, parsedPlane, parsedRefSlice, parsedTarSlice, parsedRefA, parsedRefB, parsedTarA, parsedTarB);
  return parameters;
}

int ParametersParser::parseDims(string dims)
{
  if(dims.compare("1D") == 0)
    return 1;
  if(dims.compare("2D") == 0)
    return 2;
  if(dims.compare("3D") == 0)
    return 3;
  if(dims.compare("2.5D") == 0)
    return 4;
  if(dims.compare("2D From 3D") == 0)
    return 5;
  return -1;
}

bool ParametersParser::parseLocal(string local)
{
  return local.compare("Local") == 0;
}
