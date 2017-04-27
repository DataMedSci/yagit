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

#ifndef GI_TEST_COMPARATOR_H
#define GI_TEST_COMPARATOR_H

#include <iostream>
using namespace std;

class Comparator {
public:
  virtual bool compare(double*, double*, double, int) = 0;
  virtual string name() = 0;
};

class GPRComparator : public Comparator
{
public:
  bool compare(double*, double*, double, int);
  string name();
};

class OneToOneComparator : public Comparator
{
public:
  bool compare(double*, double*, double, int);
  string name();
};

#endif
