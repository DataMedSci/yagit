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


# GAMMA CORE
CORE_PATH=../gi_core
CORE_LIB=gi_core

# GAMMA ADDITIONS
ADDITIONS_PATH=../gi_additions
ADDITIONS_LIB=gi_additions

# GAMMA WRAPPER
WRAPPER_PATH=../gi_wrapper_cpp
WRAPPER_LIB=gi_wrapper

IMEBRA_FLAGS = -I$(IMEBRA_INC_PATH) -L$(IMEBRA_LINK_PATH) -l$(IMEBRA_LIB)
WXWIDGETS_FLAGS = `wx-config --cxxflags --libs`

CORE_FLAGS = -I$(CORE_PATH) -L$(CORE_PATH) -l$(CORE_LIB)
ADDITIONS_FLAGS = -I$(ADDITIONS_PATH) -L$(ADDITIONS_PATH) -l$(ADDITIONS_LIB)
WRAPPER_FLAGS = -I$(WRAPPER_PATH) -L$(WRAPPER_PATH) -l$(WRAPPER_LIB)


build_gui:
	g++ *.h *.cpp -o yAGIT -std=c++11 $(WXWIDGETS_FLAGS) $(WRAPPER_FLAGS) $(CORE_FLAGS) $(ADDITIONS_FLAGS) $(IMEBRA_FLAGS)

clean:
	rm yAGIT
