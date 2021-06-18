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

#include <iostream>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
using namespace std;

class Parameters
{
public:
  Parameters(int dims_, double dd_, double dta_, double refVal_, bool local_, double limit_,
    int rescale_, int plane_, int refSlice_, int tarSlice_, double refA_, double refB_, double tarA_, double tarB_) :
      dims(dims_), dd(dd_), dta(dta_), refVal(refVal_), local(local_), limit(limit_),
      rescale(rescale_), plane(plane_), refSlice(refSlice_), tarSlice(tarSlice_),
      refA(refA_), refB(refB_), tarA(tarA_), tarB(tarB_) { }
  ~Parameters();

  int dims;
  double dd;
  double dta;
  double refVal;
  bool local;
  double limit;
  int rescale;
  int plane;
  int refSlice;
  int tarSlice;

  double refA;
  double refB;
  double tarA;
  double tarB;
};

class ParametersParser
{
public:
  ParametersParser() { };
  ~ParametersParser();

  Parameters* resolveParameters(wxString dims, wxString dd, wxString dta, wxString refVal,
    wxString local, wxString limit, wxString rescale, wxString plane, wxString refSlice, wxString tarSlice,
    wxString refA, wxString refB, wxString tarA, wxString tarB);
private:
  int parseDims(string dims);
  bool parseLocal(string local);
};
