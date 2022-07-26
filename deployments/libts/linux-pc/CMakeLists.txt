#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
include(../../deployment.cmake REQUIRED)

#-------------------------------------------------------------------------------
#  The CMakeLists.txt for building the libts deployment for linux-pc
#
#  Used for building the libts library for the linux-pc enviroment.  Also
#  builds the libts-test executable that acts as a client for the services
#  accessed via libts.  For the linux-pc deployment, libts contains
#  standalone versions of a set of trusted services to support client
#  application development in a native PC environment.
#-------------------------------------------------------------------------------
include(${TS_ROOT}/environments/linux-pc/env_shared_lib.cmake)
project(trusted-services LANGUAGES CXX C)
add_library(ts SHARED)
target_include_directories(ts PRIVATE "${TOP_LEVEL_INCLUDE_DIRS}")

#-------------------------------------------------------------------------------
#  Components that are specific to deployment in the linux-pc environment.
#
#-------------------------------------------------------------------------------
add_components(
	TARGET "ts"
	BASE_DIR ${TS_ROOT}
	COMPONENTS
		"components/rpc/direct"
		"components/common/tlv"
		"components/common/endian"
		"components/common/utils"
		"components/config/ramstore"
		"components/service/common/include"
		"components/service/common/client"
		"components/service/common/serializer/protobuf"
		"components/service/common/provider"
		"components/service/discovery/provider"
		"components/service/discovery/provider/serializer/packed-c"
		"components/service/locator/standalone"
		"components/service/locator/standalone/services/crypto"
		"components/service/locator/standalone/services/internal-trusted-storage"
		"components/service/locator/standalone/services/protected-storage"
		"components/service/locator/standalone/services/test-runner"
		"components/service/locator/standalone/services/attestation"
		"components/service/locator/standalone/services/smm-variable"
		"components/service/attestation/include"
		"components/service/attestation/claims"
		"components/service/attestation/claims/sources/boot_seed_generator"
		"components/service/attestation/claims/sources/null_lifecycle"
		"components/service/attestation/claims/sources/instance_id"
		"components/service/attestation/claims/sources/implementation_id"
		"components/service/attestation/claims/sources/event_log"
		"components/service/attestation/claims/sources/event_log/mock"
		"components/service/attestation/reporter/local"
		"components/service/attestation/reporter/eat"
		"components/service/attestation/key_mngr/local"
		"components/service/attestation/provider"
		"components/service/attestation/provider/serializer/packed-c"
		"components/service/crypto/include"
		"components/service/crypto/client/psa"
		"components/service/crypto/provider"
		"components/service/crypto/provider/serializer/protobuf"
		"components/service/crypto/provider/serializer/packed-c"
		"components/service/crypto/provider/extension/hash"
		"components/service/crypto/provider/extension/hash/serializer/packed-c"
		"components/service/crypto/provider/extension/cipher"
		"components/service/crypto/provider/extension/cipher/serializer/packed-c"
		"components/service/crypto/provider/extension/key_derivation"
		"components/service/crypto/provider/extension/key_derivation/serializer/packed-c"
		"components/service/crypto/provider/extension/mac"
		"components/service/crypto/provider/extension/mac/serializer/packed-c"
		"components/service/crypto/provider/extension/aead"
		"components/service/crypto/provider/extension/aead/serializer/packed-c"
		"components/service/crypto/factory/full"
		"components/service/crypto/backend/mbedcrypto"
		"components/service/crypto/backend/mbedcrypto/trng_adapter/linux"
		"components/service/secure_storage/include"
		"components/service/secure_storage/frontend/psa/its"
		"components/service/secure_storage/frontend/secure_storage_provider"
		"components/service/secure_storage/backend/secure_storage_client"
		"components/service/secure_storage/backend/mock_store"
		"components/service/secure_storage/backend/null_store"
		"components/service/test_runner/provider"
		"components/service/test_runner/provider/serializer/packed-c"
		"components/service/test_runner/provider/backend/mock"
		"components/service/test_runner/provider/backend/simple_c"
		"components/service/smm_variable/backend"
		"components/service/smm_variable/provider"
		"protocols/rpc/common/packed-c"
		"protocols/service/crypto/packed-c"
		"protocols/service/crypto/protobuf"
		"protocols/service/secure_storage/packed-c"
)

#-------------------------------------------------------------------------------
#  Extend with components that are common across all deployments of
#  libts
#
#-------------------------------------------------------------------------------
include(../libts.cmake REQUIRED)

#-------------------------------------------------------------------------------
#  Components used by libts from external projects
#
#-------------------------------------------------------------------------------

# Nanopb
include(${TS_ROOT}/external/nanopb/nanopb.cmake)
target_link_libraries(ts PRIVATE nanopb::protobuf-nanopb-static)
protobuf_generate_all(TGT "ts" NAMESPACE "protobuf" BASE_DIR "${TS_ROOT}/protocols")

# Mbed TLS provides libmbedcrypto
include(${TS_ROOT}/external/MbedTLS/MbedTLS.cmake)
target_link_libraries(ts PRIVATE mbedcrypto)

# Qcbor
include(${TS_ROOT}/external/qcbor/qcbor.cmake)
target_link_libraries(ts PRIVATE qcbor)

# t_cose
include(${TS_ROOT}/external/t_cose/t_cose.cmake)
target_link_libraries(ts PRIVATE t_cose)

#-------------------------------------------------------------------------------
#  Test executable (libts-test) for testing libts static library
#
#-------------------------------------------------------------------------------
add_executable(libts-test)
target_include_directories(libts-test PRIVATE "${TOP_LEVEL_INCLUDE_DIRS}")
target_link_libraries(libts-test PRIVATE "-Wl,--whole-archive" ts "-Wl,--no-whole-archive")

add_components(
	TARGET "libts-test"
	BASE_DIR ${TS_ROOT}
	COMPONENTS
		"components/app/test-runner"
		"components/common/tlv"
		"components/service/common/include"
		"components/service/discovery/client"
		"components/service/secure_storage/include"
		"components/service/secure_storage/test/service"
		"components/service/secure_storage/frontend/psa/its"
		"components/service/secure_storage/frontend/psa/its/test"
		"components/service/secure_storage/frontend/psa/ps"
		"components/service/secure_storage/frontend/psa/ps/test"
		"components/service/secure_storage/backend/secure_storage_client"
		"components/service/crypto/test/service"
		"components/service/crypto/test/service/protobuf"
		"components/service/crypto/test/service/packed-c"
		"components/service/crypto/include"
		"components/service/crypto/client/psa"
		"components/service/crypto/client/cpp"
		"components/service/crypto/client/cpp/protocol/protobuf"
		"components/service/crypto/client/cpp/protocol/packed-c"
		"components/service/common/serializer/protobuf"
		"components/service/common/client"
		"protocols/service/crypto/protobuf"
		"protocols/service/crypto/packed-c"
)

#-------------------------------------------------------------------------------
#  Components used by libts-test from external projects
#
#-------------------------------------------------------------------------------

# CppUTest
include(${TS_ROOT}/external/CppUTest/CppUTest.cmake)
target_link_libraries(libts-test PRIVATE CppUTest)

# Nanopb
target_link_libraries(libts-test PRIVATE nanopb::protobuf-nanopb-static)
protobuf_generate_all(TGT "libts-test" NAMESPACE "protobuf" BASE_DIR "${TS_ROOT}/protocols")

# Mbedcrypto
target_link_libraries(libts-test PRIVATE mbedcrypto)
