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

# GAMMA CORE
CORE_PATH=../gi_core
CORE_LIB=gi_core

CORE_FLAGS = -I$(CORE_PATH) -L$(CORE_PATH) -l$(CORE_LIB)


compileTestFramework:
	g++ *.h onetoonecomparator.cpp gprcomparator.cpp parser.cpp data.cpp mainTest.cpp parameters.cpp parseData.cpp parseParameters.cpp -o mainTest -std=c++11 $(CORE_FLAGS)

run:
	./mainTest

compileAndRun:
	compileTestFramework
	run

prepareSuits:
	python3 manageCompressingSuits.py decompress

cleanSuits:
	python3 manageCompressingSuits.py compress
	rm -r ./suits

transformation:
	g++ *.h transformer.cpp data.cpp parameters.cpp parseData.cpp parseParameters.cpp parser.cpp onetoonecomparator.cpp gprcomparator.cpp -o transformer -std=c++11
