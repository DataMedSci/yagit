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

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)
find_qrc_files("${CMAKE_CURRENT_SOURCE_DIR}/res" yAGIT_res)
find_ui_files("${CMAKE_CURRENT_SOURCE_DIR}/ui" yAGIT_uis)

find_package(Qt5 COMPONENTS Core Gui Widgets UiPlugin REQUIRED)


# WIDGETS

project(yAGITQt5Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/include/widgets" yAGITQt5Widgets_includes)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src/widgets" yAGITQt5Widgets_sources)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
	${yAGITQt5Widgets_includes}
	${yAGITQt5Widgets_sources}
	${yAGIT_res}
	${yAGIT_uis}
	)

add_library(yAGITQt5Widgets STATIC
	${yAGITQt5Widgets_includes}
	${yAGITQt5Widgets_sources}
	${yAGITQt_res}
	${yAGITQt_uis}
	)

target_link_libraries(yAGITQt5Widgets PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
target_include_directories(yAGITQt5Widgets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets")

set_target_properties(yAGITQt5Widgets
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set_default_output_directories(yAGITQt5Widgets)


# EXECUTABLE

project(yAGITQt5)

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/include/gui" yAGITQt5_includes)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src/gui" yAGITQt5_sources)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
	${yAGITQt5_includes}
	${yAGITQt5_sources}
	${yAGIT_res}
	${yAGIT_uis}
	)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

add_executable(yAGITQt5
	${yAGITQt5_includes}
	${yAGITQt5_sources}
	${yAGIT_res}
	${yAGIT_uis}
	)

target_link_libraries(yAGITQt5 PRIVATE yAGITQt5Widgets gi_core gi_additions gi_wrapper)
target_include_directories(yAGITQt5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/gui")

set_target_properties(yAGITQt5
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set_default_output_directories(yAGITQt5)


# PLUGIN

project(yAGITQt5WidgetsPlugin)

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/include/plugin" yAGITQt5WidgetsPlugin_includes)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src/plugin" yAGITQt5WidgetsPlugin_sources)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
	${yAGITQt5WidgetsPlugin_includes}
	${yAGITQt5WidgetsPlugin_sources}
	${yAGIT_res}
	${yAGIT_uis}
	)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

add_library(yAGITQt5WidgetsPlugin SHARED
	${yAGITQt5WidgetsPlugin_includes}
	${yAGITQt5WidgetsPlugin_sources}
	${yAGIT_res}
	${yAGIT_uis}
	)

target_link_libraries(yAGITQt5WidgetsPlugin PUBLIC yAGITQt5Widgets Qt5::UiPlugin)
target_include_directories(yAGITQt5WidgetsPlugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/plugin")

set_target_properties(yAGITQt5WidgetsPlugin
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set_default_output_directories(yAGITQt5WidgetsPlugin)

if(DEFINED QT5_PLUGINS_DIR)
	add_custom_target(install_yAGITQt5WidgetsPlugin
		DEPENDS yAGITQt5WidgetsPlugin
		COMMENT "Installing yAGITQt5WidgetsPlugin"
	)
	add_custom_command(TARGET install_yAGITQt5WidgetsPlugin PRE_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy
			$<TARGET_FILE_DIR:yAGITQt5WidgetsPlugin>/$<TARGET_FILE_NAME:yAGITQt5WidgetsPlugin>
			${QT5_PLUGINS_DIR}/designer/$<TARGET_FILE_NAME:yAGITQt5WidgetsPlugin>
	)
endif()
