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

cmake_minimum_required(VERSION 3.8)
project(gi_test_framework)
project(gi_test_transformation)

SET(gi_tests_headers
    comparator.h
    data.h
    parameters.h
    parseData.h
    parseParameters.h
    parser.h
)

SET(gi_test_framework_sources
    ${gi_tests_headers}
    onetoonecomparator.cpp
    gprcomparator.cpp
    parser.cpp
    data.cpp
    mainTest.cpp
    parameters.cpp
    parseData.cpp
    parseParameters.cpp
)

SET(gi_test_transformation_sources
    ${gi_tests_headers}
    transformer.cpp
    data.cpp
    parameters.cpp
    parseData.cpp
    parseParameters.cpp
    parser.cpp
    onetoonecomparator.cpp
    gprcomparator.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${gi_tests_sources})

add_executable(gi_test_framework ${gi_test_framework_sources})
target_link_libraries(gi_test_framework PUBLIC gi_core)
target_include_directories(gi_test_framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(gi_test_transformation ${gi_test_transformation_sources})
target_include_directories(gi_test_transformation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(gi_test_framework
    PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set_target_properties(gi_test_transformation
    PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set_default_output_directories(gi_test_framework)
set_default_output_directories(gi_test_transformation)
