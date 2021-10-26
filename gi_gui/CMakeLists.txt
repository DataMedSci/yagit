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
project(yAGIT)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

find_package(GDCM CONFIG REQUIRED)

find_package(wxWidgets COMPONENTS core base adv REQUIRED)
include(${wxWidgets_USE_FILE})

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}" yAGIT_sources)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${yAGIT_sources})

if(MSVC)
	add_executable(yAGIT WIN32 ${yAGIT_sources})
else()
	add_executable(yAGIT ${yAGIT_sources})
endif()

target_link_libraries(yAGIT PRIVATE gi_core gi_additions gi_wrapper ${wxWidgets_LIBRARIES} gdcmIOD gdcmDICT gdcmDSED gdcmMEXD)
target_include_directories(yAGIT PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${wxWidgets_INCLUDE_DIRS})

set_target_properties(yAGIT
    PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set_default_output_directories(yAGIT)
