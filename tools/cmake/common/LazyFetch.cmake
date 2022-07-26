#-------------------------------------------------------------------------------
# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Determine the number of processes to run while running parallel builds.
# Pass -DPROCESSOR_COUNT=<n> to cmake to override.
if(NOT DEFINED PROCESSOR_COUNT)
	include(ProcessorCount)
	ProcessorCount(PROCESSOR_COUNT)
	set(PROCESSOR_COUNT ${PROCESSOR_COUNT} CACHE STRING "Number of cores to use for parallel builds.")
endif()

#[===[.rst:
Common fetch interface for external dependencies.
-------------------------------------------------
#]===]

#[===[.rst:
.. cmake:command:: LazyFetch_Fetch

	.. code:: cmake
	LazyFetch_Fetch(DEP_NAME <dependency name> OPTIONS <list of options for FetchContent_Declare>)

	INPUTS:
	``DEP_NAME``
	Unique name for the dependency, used by FetchContent_* functions
	``OPTIONS``
	List of options for FetchContent_Declare, e.g. git url and refspec, check cmake documentations for more
	information
	#]===]

function(LazyFetch_Fetch DEP_NAME OPTIONS)
	include(FetchContent)

	# Fetching dependency
	FetchContent_Declare(
		${DEP_NAME}
		${OPTIONS}
		)

	FetchContent_GetProperties(${DEP_NAME})
	if(NOT "${DEP_NAME}_POPULATED")
		message(STATUS "Fetching ${DEP_NAME}")
		FetchContent_Populate(${DEP_NAME})
	endif()
endfunction()

#[===[.rst:
.. cmake:command:: LazyFetch_ConfigAndBuild

	.. code:: cmake
	LazyFetch_ConfigAndBuild(DEP_NAME <dependency name> SRC_DIR <source code dir> BIN_DIR <binary dir>
		CACHE_FILE <path to the initial cache file> INSTALL_DIR <install path>)

	INPUTS:
	``DEP_NAME``
	Unique name for the dependency
	``SRC_DIR``
	Source directory
	``BIN_DIR``
	Build directory
	``CACHE_FILE``
	Path to the initial cache file. Setting cache variables in this file can be used to augment
	the configure process. The file goes through :cmake:function:`configure_file(... @ONLY)`, this
	can be used to pass variables to the external project.
	``INSTALL_DIR``
	Install path. If not set, the install step will be skipped.
	#]===]

function(LazyFetch_ConfigAndBuild)
	set(oneValueArgs DEP_NAME SRC_DIR BIN_DIR CACHE_FILE INSTALL_DIR)
	cmake_parse_arguments(BUILD "${__options}" "${oneValueArgs}" "${_multipleValueArgs}" ${ARGN})
	message(STATUS "Configuring and building ${BUILD_DEP_NAME}")

	# Store config file in build dir, so it gets cleaned up
	set(CONFIGURED_CACHE_FILE ${CMAKE_BINARY_DIR}/${BUILD_DEP_NAME}-init-cache.cmake)
	configure_file(${BUILD_CACHE_FILE}  ${CONFIGURED_CACHE_FILE} @ONLY)

	execute_process(COMMAND
		${CMAKE_COMMAND} -E env "CROSS_COMPILE=${CROSS_COMPILE}"
		${CMAKE_COMMAND}
			"-C${CONFIGURED_CACHE_FILE}"
			-S ${BUILD_SRC_DIR}
			-B ${BUILD_BIN_DIR}
		RESULT_VARIABLE
			_exec_error
		)
	if (NOT _exec_error EQUAL 0)
		message(FATAL_ERROR "Configuring ${BUILD_DEP_NAME} build failed. `${_exec_error}`")
	endif()

	if (BUILD_INSTALL_DIR)
		execute_process(COMMAND
			${CMAKE_COMMAND} -E env "CROSS_COMPILE=${CROSS_COMPILE}"
			${CMAKE_COMMAND}
				--build ${BUILD_BIN_DIR}
				--parallel ${PROCESSOR_COUNT}
				--target install
			RESULT_VARIABLE
				_exec_error
			)
	else()
		execute_process(COMMAND
			${CMAKE_COMMAND} -E env "CROSS_COMPILE=${CROSS_COMPILE}"
			${CMAKE_COMMAND}
				--build ${BUILD_BIN_DIR}
				--parallel ${PROCESSOR_COUNT}
			RESULT_VARIABLE
				_exec_error
			)
	endif()
	if (NOT _exec_error EQUAL 0)
		message(FATAL_ERROR "Building ${BUILD_DEP_NAME} failed. ${_exec_error}")
	endif()
endfunction()

#[===[.rst:
.. cmake:command:: LazyFetch_MakeAvailable

	.. code:: cmake
	LazyFetch_MakeAvailable(DEP_NAME <dependency name> INSTALL_DIR <install path>
		PACKAGE_DIR <directory of moduleConfig.cmake file>
		CACHE_FILE <path to the cache init file> FETCH_OPTIONS <options for the fetching process>)

	INPUTS:
	``DEP_NAME``
	If set, this path overwrites the default base path for the FetchContent process
	``SOURCE_DIR``
	Location of source code.
	``INSTALL_DIR``
	Build install path
	``PACKAGE_DIR``
	If set find_package will search this directory for the config file
	``CACHE_FILE``
	Path to the cache init file, setting cache variables in this file can be used to augment the
	configure process. The file goes through the :cmake:function:`configure_file(... @ONLY)`, this
	can be used to pass variables to the cache file
	``FETCH_OPTIONS``
	Configure the dependency fetching process, this is passed to FetchContent_Declare, check the
	cmake documentation for more info
	``SOURCE_SUBDIR``
	A subdirectory relative to the top level directory of the fetched component, where the CMakeLists.txt file
	can be found.
	#]===]

macro(LazyFetch_MakeAvailable)
	set(oneValueArgs DEP_NAME SOURCE_DIR BINARY_DIR INSTALL_DIR PACKAGE_DIR CACHE_FILE SOURCE_SUBDIR)
	set(multipleValueArgs FETCH_OPTIONS)
	cmake_parse_arguments(MY "${__options}" "${oneValueArgs}" "${multipleValueArgs}" ${ARGN})
	message(STATUS "Looking for dependency ${MY_DEP_NAME}")

	if (NOT DEFINED MY_DEP_NAME)
		message(FATAL_ERROR "Mandatory parameter DEP_NAME is missing.")
	endif()

	# FetchContent* functions use this form
	string(TOLOWER ${MY_DEP_NAME} MY_LC_DEP_NAME)

	# These two variables are also set by the normal FetchContent process and users could depend on them,
	# so they are not unset at the end of the macro
	if (MY_BINARY_DIR)
		set(${MY_LC_DEP_NAME}_BINARY_DIR ${MY_BINARY_DIR})
	else()
		set(${MY_LC_DEP_NAME}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/${MY_LC_DEP_NAME}-build CACHE PATH "Build directory for ${MY_LC_DEP_NAME}")
	endif()
	if (MY_SOURCE_DIR)
		set(${MY_LC_DEP_NAME}_SOURCE_DIR ${MY_SOURCE_DIR})
	else()
		set(${MY_LC_DEP_NAME}_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/${MY_LC_DEP_NAME}-src CACHE PATH "Source directory for ${MY_LC_DEP_NAME}")
	endif()

	set(${MY_LC_DEP_NAME}_SUBBUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/_deps/${MY_LC_DEP_NAME}-subbuild")

	list(APPEND MY_FETCH_OPTIONS
			SOURCE_DIR "${${MY_LC_DEP_NAME}_SOURCE_DIR}"
			BINARY_DIR "${${MY_LC_DEP_NAME}_BINARY_DIR}"
			SUBBUILD_DIR "${${MY_LC_DEP_NAME}_SUBBUILD_DIR}")

	if (NOT DEFINED MY_INSTALL_DIR OR NOT EXISTS ${MY_INSTALL_DIR})
		if (NOT EXISTS ${${MY_LC_DEP_NAME}_BINARY_DIR} OR NOT EXISTS ${${MY_LC_DEP_NAME}_SOURCE_DIR})
			if (NOT EXISTS ${${MY_LC_DEP_NAME}_SOURCE_DIR})
				LazyFetch_Fetch(${MY_LC_DEP_NAME} "${MY_FETCH_OPTIONS}")
				file(REMOVE_RECURSE "${${MY_LC_DEP_NAME}_BINARY_DIR}")
				file(REMOVE_RECURSE "${${MY_LC_DEP_NAME}_SUBBUILD_DIR}")
			endif()
			if (MY_CACHE_FILE)
				LazyFetch_ConfigAndBuild(
					DEP_NAME ${MY_LC_DEP_NAME}
					SRC_DIR ${${MY_LC_DEP_NAME}_SOURCE_DIR}/${MY_SOURCE_SUBDIR}
					BIN_DIR ${${MY_LC_DEP_NAME}_BINARY_DIR}
					CACHE_FILE ${MY_CACHE_FILE}
					INSTALL_DIR ${MY_INSTALL_DIR}
					)
			endif()
		elseif(DEFINED MY_INSTALL_DIR)
			execute_process(COMMAND
				${CMAKE_COMMAND} -E env "CROSS_COMPILE=${CROSS_COMPILE}"
				${CMAKE_COMMAND}
					--build ${${MY_LC_DEP_NAME}_BINARY_DIR}
					--parallel ${PROCESSOR_COUNT}
					--target install
				RESULT_VARIABLE
					_exec_error
				)
			if (NOT _exec_error EQUAL 0)
				message(FATAL_ERROR "Installing ${BUILD_DEP_NAME} failed. ${_exec_error}")
			endif()
		endif()
	endif()

	# Run find_package again if we just needed the build and install step
	if (MY_PACKAGE_DIR)
		unset(${MY_LC_DEP_NAME}_DIR)
		unset(${MY_LC_DEP_NAME}_DIR CACHE)
		unset(${MY_DEP_NAME}-FOUND CACHE)
		find_package(${MY_DEP_NAME} CONFIG REQUIRED NO_DEFAULT_PATH PATHS ${MY_PACKAGE_DIR})
		set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${MY_DEP_NAME}_CONFIG)
	endif()

	unset(MY_DEP_NAME)
	unset(MY_SOURCE_DIR)
	unset(MY_BINARY_DIR)
	unset(MY_INSTALL_DIR)
	unset(MY_PACKAGE_DIR)
	unset(MY_CACHE_FILE)
	unset(MY_SOURCE_SUBDIR)
	unset(MY_FETCH_OPTIONS)
	unset(MY_LC_DEP_NAME)
	unset(oneValueArgs)
	unset(multipleValueArgs)
endmacro()