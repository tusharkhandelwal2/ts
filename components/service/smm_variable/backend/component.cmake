#-------------------------------------------------------------------------------
# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
if (NOT DEFINED TGT)
	message(FATAL_ERROR "mandatory parameter TGT is not defined.")
endif()

target_sources(${TGT} PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}/uefi_variable_store.c"
	"${CMAKE_CURRENT_LIST_DIR}/variable_index.c"
	"${CMAKE_CURRENT_LIST_DIR}/variable_index_iterator.c"
	"${CMAKE_CURRENT_LIST_DIR}/variable_checker.c"
	)
