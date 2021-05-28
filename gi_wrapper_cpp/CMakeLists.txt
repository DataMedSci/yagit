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
project(gi_wrapper)

set(IMEBRA_SHARED_STATIC STATIC)

CPMFindPackage(
	NAME imebra
	GITHUB_REPOSITORY binarno/imebra_mirror
	GIT_TAG 4.0.8
	VERSION 4.0.8
	DOWNLOAD_ONLY YES
)

if (imebra_ADDED)
	add_subdirectory(${imebra_SOURCE_DIR}/library ${imebra_BINARY_DIR})
endif()

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}" gi_wrapper_sources)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${gi_wrapper_sources})

add_library(gi_wrapper STATIC ${gi_wrapper_sources})
target_link_libraries(gi_wrapper PUBLIC imebra)
target_include_directories(gi_wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(gi_wrapper
    PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set_default_output_directories(gi_wrapper)
