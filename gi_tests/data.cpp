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

#include "data.h"
#include <iostream>
using namespace std;

void Data::printInfo()
{
  cout << "startX: " << startX << endl;
  cout << "spacingX: " << spacingX << endl;
  cout << "numberX: " << numberX << endl;
  cout << "startY: " << startX << endl;
  cout << "spacingY: " << spacingY << endl;
  cout << "numberY: " << numberY << endl;
  cout << "startZ: " << startZ << endl;
  cout << "spacingZ: " << spacingZ << endl;
  cout << "numberZ: " << numberZ << endl;
}

void Data::printData()
{
  int size = getDataSize();

  for(int i=0; i < size; i++)
  {
    cout << data[i] << endl;
  }
}

int Data::getDataSize()
{
  int dataSize = numberX;
  if(numberY != 0)
  {
    dataSize *= numberY;
  }
  if(numberZ != 0)
  {
    dataSize *= numberZ;
  }

  return dataSize;
}

Data::~Data()
{
  delete[] data;
}
