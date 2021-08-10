/*
 * Copyright (c) 2020-2021, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PROTOBUF_CRYPTO_CLIENT_H
#define PROTOBUF_CRYPTO_CLIENT_H

#include <service/crypto/client/cpp/crypto_client.h>
#include <service/crypto/protobuf/key_attributes.pb.h>

/*
 * A concrete crypto_client that uses the protobuf based crypto access protocol
 */
class protobuf_crypto_client : public crypto_client
{
public:
	protobuf_crypto_client();
	protobuf_crypto_client(struct rpc_caller *caller);
	virtual ~protobuf_crypto_client();

	/* Key lifecycle methods */
	psa_status_t generate_key(
		const psa_key_attributes_t *attributes,
		psa_key_id_t *id);

	psa_status_t destroy_key(
		psa_key_id_t id);

	psa_status_t import_key(
		const psa_key_attributes_t *attributes,
		const uint8_t *data, size_t data_length, psa_key_id_t *id);

	psa_status_t copy_key(
		psa_key_id_t source_key,
		const psa_key_attributes_t *attributes,
		psa_key_id_t *target_key);

	psa_status_t purge_key(
		psa_key_id_t id);

	psa_status_t get_key_attributes(
		psa_key_id_t id,
		psa_key_attributes_t *attributes);

	/* Key export methods */
	psa_status_t export_key(
		psa_key_id_t id,
		uint8_t *data, size_t data_size,
		size_t *data_length);

	psa_status_t export_public_key(
		psa_key_id_t id,
		uint8_t *data, size_t data_size, size_t *data_length);

	/* Sign/verify methods */
	psa_status_t sign_hash(
		psa_key_id_t id,
		psa_algorithm_t alg,
		const uint8_t *hash, size_t hash_length,
		uint8_t *signature, size_t signature_size, size_t *signature_length);

	psa_status_t verify_hash(
		psa_key_id_t id,
		psa_algorithm_t alg,
		const uint8_t *hash, size_t hash_length,
		const uint8_t *signature, size_t signature_length);

	/* Asymmetric encrypt/decrypt */
	psa_status_t asymmetric_encrypt(
		psa_key_id_t id,
		psa_algorithm_t alg,
		const uint8_t *input, size_t input_length,
		const uint8_t *salt, size_t salt_length,
		uint8_t *output, size_t output_size, size_t *output_length);

	psa_status_t asymmetric_decrypt(
		psa_key_id_t id,
		psa_algorithm_t alg,
		const uint8_t *input, size_t input_length,
		const uint8_t *salt, size_t salt_length,
		uint8_t *output, size_t output_size, size_t *output_length);

	/* Random number generation */
	psa_status_t generate_random(
		uint8_t *output, size_t output_size);

	/* Hash methods */
	size_t hash_max_update_size() const;

	psa_status_t hash_setup(
		uint32_t *op_handle,
		psa_algorithm_t alg);

	psa_status_t hash_update(
		uint32_t op_handle,
		const uint8_t *input, size_t input_length);

	psa_status_t hash_finish(
		uint32_t op_handle,
		uint8_t *hash, size_t hash_size, size_t *hash_length);

	psa_status_t hash_abort(
		uint32_t op_handle);

	psa_status_t hash_verify(
		uint32_t op_handle,
		const uint8_t *hash, size_t hash_length);

	psa_status_t hash_clone(
		uint32_t source_op_handle,
		uint32_t *target_op_handle);

	/* Cipher methods */
	size_t cipher_max_update_size() const;

	psa_status_t cipher_encrypt_setup(
		uint32_t *op_handle,
		psa_key_id_t key,
		psa_algorithm_t alg);

	psa_status_t cipher_decrypt_setup(
		uint32_t *op_handle,
		psa_key_id_t key,
		psa_algorithm_t alg);

	psa_status_t cipher_generate_iv(
		uint32_t op_handle,
		uint8_t *iv, size_t iv_size, size_t *iv_length);

	psa_status_t cipher_set_iv(
		uint32_t op_handle,
		const uint8_t *iv, size_t iv_length);

	psa_status_t cipher_update(
		uint32_t op_handle,
		const uint8_t *input, size_t input_length,
		uint8_t *output, size_t output_size, size_t *output_length);

	psa_status_t cipher_finish(
		uint32_t op_handle,
		uint8_t *output, size_t output_size, size_t *output_length);

	psa_status_t cipher_abort(
		uint32_t op_handle);

	/* MAC methods */
	size_t mac_max_update_size() const;

	psa_status_t mac_sign_setup(
		uint32_t *op_handle,
		psa_key_id_t key,
		psa_algorithm_t alg);

	psa_status_t mac_verify_setup(
		uint32_t *op_handle,
		psa_key_id_t key,
		psa_algorithm_t alg);

	psa_status_t mac_update(
		uint32_t op_handle,
		const uint8_t *input, size_t input_length);

	psa_status_t mac_sign_finish(
		uint32_t op_handle,
		uint8_t *mac, size_t mac_size, size_t *mac_length);

	psa_status_t mac_verify_finish(
		uint32_t op_handle,
		const uint8_t *mac, size_t mac_length);

	psa_status_t mac_abort(
		uint32_t op_handle);


private:

	void translate_key_attributes(
		ts_crypto_KeyAttributes &proto_attributes,
		const psa_key_attributes_t &psa_attributes);
};

#endif /* PROTOBUF_CRYPTO_CLIENT_H */
