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

#include "parser.h"
#include "comparator.h"

using namespace std;

int Parser::startsWith(string source, string prefix)
{
  if(source.compare(0, prefix.size(), prefix) == 0)
    return true;
  return false;
}

double Parser::getDoubleAttribute(string line, string attributeName)
{
  if(startsWith(line, attributeName))
  {
    string value = line.substr(attributeName.size() + 1, line.size() - attributeName.size() - 1);
    return stod(value);
  } else
  {
    cout << "Problem in " << attributeName << endl;
    return -1.0;
  }
}

int Parser::getIntAttribute(string line, string attributeName)
{
  if(startsWith(line, attributeName))
  {
    string value = line.substr(attributeName.size() + 1, line.size() - attributeName.size() - 1);
    return stoi(value);
  } else
  {
    cout << "Problem in " << attributeName << endl;
    return -1;
  }
}

bool Parser::getIsLocalAttribute(string line, string keyword)
{
  if(startsWith(line, keyword))
  {
    string value = line.substr(keyword.size() + 1, line.size() - keyword.size() - 1);
    if(value.compare("true") == 0)
      return true;
    else if(value.compare("false") == 0)
      return false;
    else
    {
      cout << "Wrong local flag value" << endl;
      return false;
    }
  } else
  {
    cout << "Problem in " << keyword << endl;
    return -1;
  }
}

Comparator* Parser::getComparator(string line, string keyword)
{
  if(startsWith(line, keyword))
  {
    string value = line.substr(keyword.size() + 1, line.size() - keyword.size() - 1);
    if(value.compare("onetoonecomparator") == 0)
      return new OneToOneComparator();
    else if(value.compare("gpr") == 0)
      return new GPRComparator();

    cout << "Undestooded comparator" << endl;
    exit(1);


  } else
  {
    cout << "Problem in " << keyword << endl;
    return NULL;
  }
}
