#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Import libts into a dependent in-tree deployment build.  Where another
# deployment uses libts, including this file in the dependent deployment
# CMake build file allows libts to be built and installed into the binary
# directory of the dependent.
#-------------------------------------------------------------------------------
set(LIBTS_INSTALL_PATH "${CMAKE_CURRENT_BINARY_DIR}/libts_install" CACHE PATH "libts installation directory")
set(LIBTS_PACKAGE_PATH "${LIBTS_INSTALL_PATH}/lib/cmake" CACHE PATH "libts CMake package directory")
set(LIBTS_SOURCE_DIR "${TS_ROOT}/deployments/libts/${TS_ENV}" CACHE PATH "libts source directory")
set(LIBTS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/_deps/libts-build" CACHE PATH "libts binary directory")

file(MAKE_DIRECTORY ${LIBTS_BINARY_DIR})

#Configure the library
execute_process(COMMAND
	${CMAKE_COMMAND}
		-DCMAKE_INSTALL_PREFIX=${LIBTS_INSTALL_PATH}
		-GUnix\ Makefiles
		${LIBTS_SOURCE_DIR}
	WORKING_DIRECTORY
		${LIBTS_BINARY_DIR}
)

if (_exec_error)
	message(FATAL_ERROR "Configuration step of libts failed with ${_exec_error}.")
endif()

#Build the library
execute_process(COMMAND
	${CMAKE_COMMAND} --build ${LIBTS_BINARY_DIR} -- install -j8
	RESULT_VARIABLE _exec_error
)

if (_exec_error)
	message(FATAL_ERROR "Build step of libts failed with ${_exec_error}.")
endif()

# Import the built library
include(${LIBTS_INSTALL_PATH}/lib/cmake/libts_targets.cmake)
add_library(libts SHARED IMPORTED)
set_property(TARGET libts PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${LIBTS_INSTALL_PATH}/include")
set_property(TARGET libts PROPERTY IMPORTED_LOCATION "${LIBTS_INSTALL_PATH}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}ts${CMAKE_SHARED_LIBRARY_SUFFIX}")