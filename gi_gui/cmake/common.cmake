#common functions used in project configuration/generation

function(find_c_files path found_files)

	file(GLOB_RECURSE found_files_internal RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
		"${path}/*.h"
		"${path}/*.c"
	)

	set(${found_files} ${found_files_internal} PARENT_SCOPE)

endfunction()

function(find_cpp_files path found_files)

	file(GLOB_RECURSE found_files_internal RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
		"${path}/*.hpp"
		"${path}/*.inl"
		"${path}/*.cpp"
	)

	set(${found_files} ${found_files_internal} PARENT_SCOPE)

endfunction()

function(find_c_and_cpp_files path found_files)

	find_c_files(${path} found_files_internal1)
	find_cpp_files(${path} found_files_internal2)

	set(${found_files} ${found_files_internal1} ${found_files_internal2} PARENT_SCOPE)

endfunction()

function(set_default_output_directories proj_name)

	set_target_properties(${proj_name} PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/debug"
		LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/debug"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/debug"
		ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release"
		LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release"
		ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/relwithdebinfo"
		LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/relwithdebinfo"
		RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/relwithdebinfo"
		ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/minsizerel"
		LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/minsizerel"
		RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/minsizerel"
	)

endfunction()
