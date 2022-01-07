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
project(gi_core_performance_analysis)

enable_testing()

find_package(OpenCL REQUIRED)

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}" gi_core_performance_analysis_sources)

add_executable(gi_core_performance_analysis ${gi_core_performance_analysis_sources})
target_link_libraries(gi_core_performance_analysis PRIVATE gi_core ${OpenCL_LIBRARIES})
target_include_directories(gi_core_performance_analysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCL_INCLUDE_DIRS})

set_target_properties(gi_core_performance_analysis
    PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set_property(TARGET gi_core_performance_analysis PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set_default_output_directories(gi_core_performance_analysis)