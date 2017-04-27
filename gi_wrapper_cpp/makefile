# This file is part of 'yet Another Gamma Index Tool'.
#
# 'yet Another Gamma Index Tool' is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# 'yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with 'yet Another Gamma Index Tool'; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

# IMEBRA
IMEBRA_INC_PATH=../imebra/library/include
IMEBRA_LINK_PATH=../imebra/artifacts
IMEBRA_LIB=imebra

IMEBRA_FLAGS = -I$(IMEBRA_INC_PATH) -L$(IMEBRA_LINK_PATH) -l$(IMEBRA_LIB)


wrapper:
	g++ -std=c++11 -c *.cpp $(IMEBRA_FLAGS)
	ar rcs libgi_wrapper.a *.o
	rm *.o

clean:
	rm libgi_wrapper.a
