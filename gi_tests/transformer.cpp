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
#include <string>
#include <fstream>
#include "parseData.h"
using namespace std;

void saveTransformedToFile(double* core, string path, int number)
{
  ofstream resultFile (path);

  for(int i=0; i < number; i++)
  {
    resultFile << core[i] << endl;
  }

  resultFile.close();
}

string* split(string s, const char* c)
{
  int cNumber = 0;
  for(int i=0; i<s.size(); i++){
    if(s.at(i) == *c)
      cNumber++;
  }

  string* array = new string[cNumber + 1];
  int arrayIndex = 0;
  int pointer = 0;
  int len = 0;
  for(int i=0; i<s.size(); i++)
  {
    if(s.at(i) == *c){
      array[arrayIndex] = s.substr(pointer, len);
      arrayIndex++;
      len = 0;
      pointer = i + 1;
    } else
    {
      len++;
    }
  }
  array[arrayIndex] = s.substr(pointer, len);

  return array;
}

void transform2D(Data* data, string folderPath)
{
  string filePath = folderPath + "/csvs/0.csv";
  ifstream file;
  file.open(filePath.c_str());

  int xSize = data->numberX;
  int ySize = data->numberY;

  double *core = new double[xSize * ySize];

  string boofor;
  if(file.is_open())
  {

    int corePointer = 0;
    while(!file.eof())
    {
      getline(file, boofor);

      if(boofor.length() != 0)
      {
        string* array = split(boofor, ";");

        for(int i = 0; i < xSize; i++)
        {
          core[corePointer] = stod(array[i]);
          corePointer++;
        }
      }
    }

  saveTransformedToFile(core, folderPath + "/data.txt", xSize * ySize);
  file.close();
  } else
  {
    cout << "Cannot open " + filePath + ". Probably id doesn't exists" << endl;
    exit(1);
  }
}

void transform3D(Data* data, string folderPath)
{

}

void transform(string folderPath)
{
  string infoPath = folderPath + "/info.txt";

  ParseData* dataParser = new ParseData();
  Data* data = dataParser->parseOnlyInfoForData(infoPath);

  if(data->numberZ == 0)
  {
    transform2D(data, folderPath);
  } else
  {
    transform3D(data, folderPath);
  }
}

int main(int argc, char const *argv[])
{
  if(argc != 2)
  {
    cout << "ERROR: One argument (path for folder with data) is required" << endl;
    exit(-1);
  }

  string folderPath = argv[1];
  transform(folderPath);
  return 0;
}
