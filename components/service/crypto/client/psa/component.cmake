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
	"${CMAKE_CURRENT_LIST_DIR}/psa_crypto_client.c"
	"${CMAKE_CURRENT_LIST_DIR}/psa_crypto_client_key_attributes.c"
	"${CMAKE_CURRENT_LIST_DIR}/psa_asymmetric_decrypt.c"
	"${CMAKE_CURRENT_LIST_DIR}/psa_asymmetric_encrypt.c"
	"${CMAKE_CURRENT_LIST_DIR}/psa_destroy_key.c"
	"${CMAKE_CURRENT_LIST_DIR}/psa_export_key.c"
	"${CMAKE_CURRENT_LIST_DIR}/psa_export_public_key.c"
	"${CMAKE_CURRENT_LIST_DIR}/psa_generate_key.c"
	"${CMAKE_CURRENT_LIST_DIR}/psa_generate_random.c"
	"${CMAKE_CURRENT_LIST_DIR}/psa_import_key.c"
	"${CMAKE_CURRENT_LIST_DIR}/psa_sign_hash.c"
	"${CMAKE_CURRENT_LIST_DIR}/psa_verify_hash.c"
	)
