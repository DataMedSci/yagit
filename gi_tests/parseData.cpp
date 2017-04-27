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

#include "parseData.h"

Data* ParseData::parseDataFromFile(string path)
{
  if(path == "")
    return nullptr;

  string infoPath = path + "/info.txt";
  string dataPath = path + "/data.txt";

  Data* data = new Data();
  parseInfoForData(data, infoPath);
  parseDataForData(data, dataPath);
  return data;
  }

void ParseData::parseInfoForData(Data* data, string path)
{

  ifstream infoFile;
  infoFile.open(path.c_str());

  string boofor;

  if(infoFile.is_open())
  {
    getline(infoFile, boofor);
    data->startX = getDoubleAttribute(boofor, "startX");

    getline(infoFile, boofor);
    data->spacingX = getDoubleAttribute(boofor, "spacingX");

    getline(infoFile, boofor);
    data->numberX = getIntAttribute(boofor, "numberX");

    getline(infoFile, boofor);
    data->startY = getDoubleAttribute(boofor, "startY");

    getline(infoFile, boofor);
    data->spacingY = getDoubleAttribute(boofor, "spacingY");

    getline(infoFile, boofor);
    data->numberY = getIntAttribute(boofor, "numberY");

    getline(infoFile, boofor);
    data->startZ = getDoubleAttribute(boofor, "startZ");

    getline(infoFile, boofor);
    data->spacingZ = getDoubleAttribute(boofor, "spacingZ");

    getline(infoFile, boofor);
    data->numberZ = getIntAttribute(boofor, "numberZ");

    infoFile.close();
  } else
  {
    cout << "Problem in openning info data in " + path << endl;
    exit(1);
  }
}

void ParseData::parseDataForData(Data* data, string path)
{

  ifstream dataFile;
  dataFile.open(path.c_str());

  string boofor;

  if(dataFile.is_open())
  {
    int dataSize = data->getDataSize();

    double* datas = new double[dataSize];

    int dataIndex = 0;

    string boofor;
    while(!dataFile.eof())
    {
      getline(dataFile, boofor);
      if(boofor.size() != 0)
      {
        if(dataIndex >= dataSize)
        {
          cout << "Wrong number of declared data points in " + path << endl;
          exit(2);
        }
        datas[dataIndex] = stod(boofor);
        dataIndex++;
      }
    }

    data->data = datas;
    dataFile.close();
  }
}

Data* ParseData::parseOnlyInfoForData(string path)
{
  Data* data = new Data();
  parseInfoForData(data, path);
  return data;
}
