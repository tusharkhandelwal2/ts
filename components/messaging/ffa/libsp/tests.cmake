#
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

include(UnitTest)

unit_test_add_suite(
	NAME libsp_mock_assert
	SOURCES
		${CMAKE_CURRENT_LIST_DIR}/test/mock_assert.cpp
		${CMAKE_CURRENT_LIST_DIR}/test/test_mock_assert.cpp
	INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_LIST_DIR}/include/
		${PROJECT_PATH}/components/common/utils/include
	COMPILE_DEFINITIONS
		-DARM64
)

unit_test_add_suite(
	NAME libsp_mock_ffa_internal_api
	SOURCES
		${CMAKE_CURRENT_LIST_DIR}/test/mock_ffa_internal_api.cpp
		${CMAKE_CURRENT_LIST_DIR}/test/test_mock_ffa_internal_api.cpp
	INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_LIST_DIR}/include/
		${PROJECT_PATH}/components/common/utils/include
	COMPILE_DEFINITIONS
		-DARM64
)

unit_test_add_suite(
	NAME libsp_ffa
	SOURCES
		${CMAKE_CURRENT_LIST_DIR}/test/test_ffa_api.cpp
		${CMAKE_CURRENT_LIST_DIR}/test/test_ffa_memory_descriptors.cpp
		${CMAKE_CURRENT_LIST_DIR}/test/mock_ffa_internal_api.cpp
		${CMAKE_CURRENT_LIST_DIR}/ffa.c
		${CMAKE_CURRENT_LIST_DIR}/ffa_memory_descriptors.c
		${CMAKE_CURRENT_LIST_DIR}/test/mock_assert.cpp
	INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_LIST_DIR}/include/
		${PROJECT_PATH}/components/common/utils/include
	COMPILE_DEFINITIONS
		-DARM64
)

unit_test_add_suite(
	NAME libsp_mock_ffa_api
	SOURCES
		${CMAKE_CURRENT_LIST_DIR}/test/test_mock_ffa_api.cpp
		${CMAKE_CURRENT_LIST_DIR}/test/mock_ffa_api.cpp
	INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_LIST_DIR}/include/
		${PROJECT_PATH}/components/common/utils/include
	COMPILE_DEFINITIONS
		-DARM64
)

unit_test_add_suite(
	NAME libsp_sp_rxtx
	SOURCES
		${CMAKE_CURRENT_LIST_DIR}/test/test_sp_rxtx.cpp
		${CMAKE_CURRENT_LIST_DIR}/test/mock_ffa_api.cpp
		${CMAKE_CURRENT_LIST_DIR}/sp_rxtx.c
	INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_LIST_DIR}/include/
		${PROJECT_PATH}/components/common/utils/include
	COMPILE_DEFINITIONS
		-DARM64
)

unit_test_add_suite(
	NAME libsp_mock_sp_rxtx
	SOURCES
		${CMAKE_CURRENT_LIST_DIR}/test/test_mock_sp_rxtx.cpp
		${CMAKE_CURRENT_LIST_DIR}/test/mock_sp_rxtx.cpp
	INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_LIST_DIR}/include/
		${PROJECT_PATH}/components/common/utils/include
	COMPILE_DEFINITIONS
		-DARM64
)

unit_test_add_suite(
	NAME libsp_sp_discovery
	SOURCES
		${CMAKE_CURRENT_LIST_DIR}/test/test_sp_discovery.cpp
		${CMAKE_CURRENT_LIST_DIR}/sp_discovery.c
		${CMAKE_CURRENT_LIST_DIR}/test/mock_ffa_api.cpp
		${CMAKE_CURRENT_LIST_DIR}/test/mock_sp_rxtx.cpp
	INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_LIST_DIR}/include/
		${PROJECT_PATH}/components/common/utils/include
	COMPILE_DEFINITIONS
		-DARM64
)

unit_test_add_suite(
	NAME libsp_sp_memory_management
	SOURCES
		${CMAKE_CURRENT_LIST_DIR}/test/test_sp_memory_management.cpp
		${CMAKE_CURRENT_LIST_DIR}/sp_memory_management.c
		${CMAKE_CURRENT_LIST_DIR}/ffa_memory_descriptors.c
		${CMAKE_CURRENT_LIST_DIR}/test/mock_assert.cpp
		${CMAKE_CURRENT_LIST_DIR}/test/mock_ffa_api.cpp
		${CMAKE_CURRENT_LIST_DIR}/test/mock_sp_rxtx.cpp
	INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_LIST_DIR}/include/
		${PROJECT_PATH}/components/common/utils/include
	COMPILE_DEFINITIONS
		-DARM64
)

unit_test_add_suite(
	NAME libsp_sp_memory_management_internals
	SOURCES
		${CMAKE_CURRENT_LIST_DIR}/test/test_sp_memory_management_internals.cpp
		${CMAKE_CURRENT_LIST_DIR}/test/sp_memory_management_internals.yml
		${CMAKE_CURRENT_LIST_DIR}/ffa_memory_descriptors.c
		${CMAKE_CURRENT_LIST_DIR}/test/mock_assert.cpp
	INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_LIST_DIR}/include/
		${PROJECT_PATH}/components/common/utils/include
	COMPILE_DEFINITIONS
		-DARM64
)