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
#include <fstream>
#include <string>
#include "parseParameters.h"
#include "parseData.h"
#include "dispatcher.h"
#include "comparator.h"

using namespace std;

bool test(Parameters* params, Data* reference, Data* target, Data* potencialResult)
{
  int noDimentions = params->noDimentions;
  double perc = params->perc;
  double dta = params->dta;
  bool local = params->local;
  double refval = params->refval;
  double limit = params->limit;
  int plane = params->plane;
  int refslice = params->refslice;
  int tarslice = params->tarslice;

  double epsilon = params->epsilon;
  int resultSize = reference->getDataSize();
  Comparator* comparator = params->cmpObj;

  double refStartX = reference->startX;
  double refSpacingX = reference->spacingX;
  int refNumberX = reference->numberX;

  double refStartY = reference->startY;
  double refSpacingY = reference->spacingY;
  int refNumberY = reference->numberY;

  double refStartZ = reference->startZ;
  double refSpacingZ = reference->spacingZ;
  int refNumberZ = reference->numberZ;

  double* refData = reference->data;


  double tarStartX = target->startX;
  double tarSpacingX = target->spacingX;
  int tarNumberX = target->numberX;

  double tarStartY = target->startY;
  double tarSpacingY = target->spacingY;
  int tarNumberY = target->numberY;

  double tarStartZ = target->startZ;
  double tarSpacingZ = target->spacingZ;
  int tarNumberZ = target->numberZ;

  double* tarData = target->data;

  double* calculated = calculateGamma(2, noDimentions, refData, tarData,
  refStartX, refSpacingX, refNumberX,
  refStartY, refSpacingY, refNumberY,
  refStartZ, refSpacingZ, refNumberZ,
  tarStartX, tarSpacingX, tarNumberX,
  tarStartY, tarSpacingY, tarNumberY,
  tarStartZ, tarSpacingZ, tarNumberZ,
  perc, dta, local, refval, limit, plane, refslice, tarslice);

  double* referenceResult;

  if(potencialResult == nullptr)
  {
    referenceResult = calculateGamma(0, noDimentions, refData, tarData,
    refStartX, refSpacingX, refNumberX,
    refStartY, refSpacingY, refNumberY,
    refStartZ, refSpacingZ, refNumberZ,
    tarStartX, tarSpacingX, tarNumberX,
    tarStartY, tarSpacingY, tarNumberY,
    tarStartZ, tarSpacingZ, tarNumberZ,
    perc, dta, local, refval, limit, plane, refslice, tarslice);
  }
  else
  {
    referenceResult = potencialResult->data;
  }

  bool passed = comparator->compare(calculated, referenceResult, epsilon, resultSize);

  delete[] calculated;
  delete[] referenceResult;

  return passed;
}

bool handleTestCase(ParseParameters* parametersParser, ParseData* dataParser, string path)
{
  ifstream testCasePaths;
  testCasePaths.open(path.c_str());

  if(testCasePaths.is_open())
  {

    string referencePath;
    string targetPath;
    string configurationPath;
    string potencialResultPath = "";

    getline(testCasePaths, referencePath);
    getline(testCasePaths, targetPath);
    getline(testCasePaths, configurationPath);
    if(!testCasePaths.eof())
    {
      getline(testCasePaths, potencialResultPath);
    }

    Parameters* params = parametersParser->parseGIParametersFromFile(configurationPath);
    Data* reference = dataParser->parseDataFromFile(referencePath);
    Data* target = dataParser->parseDataFromFile(targetPath);
    Data* potencialResult = dataParser->parseDataFromFile(potencialResultPath);

    bool result = test(params, reference, target, potencialResult);

    if(result)
    {
      // cout << "Test " + path + " has passed" << endl;
    } else
    {
      cout << "Test " + path + " has failed" << endl;
    }

    delete params;
    delete reference;
    delete target;

    testCasePaths.close();
    return result;

  } else
  {
    cout << "Problem in openning test-case:\"";
    cout << path;
    cout << "\"" << endl;

    return false;
  }
}

void handleTestSuit(ParseParameters* parametersParser, ParseData* dataParser, string path)
{
  ifstream testSuitFile;
  testSuitFile.open(path);

  cout << "Executing " << path << " test suite" << endl;
  string boofor;
  bool result;
  int passed = 0;
  int taken = 0;
  if(testSuitFile.is_open())
  {
    while(!testSuitFile.eof())
    {
      getline(testSuitFile, boofor);
      if(boofor.length() != 0)
      {

        taken++;
        result = handleTestCase(parametersParser, dataParser, boofor);

        if(result)
          passed++;
      }
    }

    cout << passed << " out of " << taken << " tests has passed" << endl;
    testSuitFile.close();

  } else
  {
    cout << "Problem in openning test-suite file" << endl;
  }
}

int main()
{

  ParseParameters* parametersParser = new ParseParameters();
  ParseData* dataParser = new ParseData();

  string suitsPoolPath = "./suits/test-suites-pool.txt";
  string boofor;

  ifstream suitsPool;
  suitsPool.open(suitsPoolPath);

  if(suitsPool.is_open())
  {
    while(!suitsPool.eof())
    {
      getline(suitsPool, boofor);
      if(boofor.length() != 0)
        handleTestSuit(parametersParser, dataParser, boofor);
    }
  }

  delete parametersParser;
  delete dataParser;

  return 0;
}
