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
project(gi_gdcm_wrapper)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

# GI_GDCM_WRAPPER

find_package(GDCM CONFIG REQUIRED)

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/include" gi_gdcm_wrapper_headers)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src" gi_gdcm_wrapper_sources)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/compat" gi_gdcm_wrapper_compatibility_sources)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${gi_gdcm_wrapper_headers} ${gi_gdcm_wrapper_sources} ${gi_gdcm_wrapper_compatibility_sources})

add_library(gi_gdcm_wrapper STATIC ${gi_gdcm_wrapper_headers} ${gi_gdcm_wrapper_sources} ${gi_gdcm_wrapper_compatibility_sources})
target_link_libraries(gi_gdcm_wrapper PRIVATE gi_core gdcmCommon gdcmIOD gdcmDICT gdcmMSFF gdcmDSED gdcmMEXD)
target_include_directories(gi_gdcm_wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/compat)
target_include_directories(gi_gdcm_wrapper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(gi_gdcm_wrapper
    PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set_default_output_directories(gi_gdcm_wrapper)

# GI_GDCM_WRAPPER_TESTS
add_subdirectory(tests)