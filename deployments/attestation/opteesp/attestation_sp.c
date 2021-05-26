/*
 * Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <rpc/ffarpc/caller/sp/ffarpc_caller.h>
#include <rpc/ffarpc/endpoint/ffarpc_call_ep.h>
#include <protocols/rpc/common/packed-c/status.h>
#include <config/ramstore/config_ramstore.h>
#include <config/loader/sp/sp_config_loader.h>
#include <service/attestation/provider/attest_provider.h>
#include <service/attestation/provider/serializer/packed-c/packedc_attest_provider_serializer.h>
#include <service/attestation/claims/claims_register.h>
#include <service/attestation/claims/sources/event_log/event_log_claim_source.h>
#include <service/attestation/claims/sources/boot_seed_generator/boot_seed_generator.h>
#include <service/attestation/claims/sources/null_lifecycle/null_lifecycle_claim_source.h>
#include <service/attestation/claims/sources/instance_id/instance_id_claim_source.h>
#include <ffa_api.h>
#include <sp_api.h>
#include <sp_rxtx.h>
#include <trace.h>


/* Temporary dependencies */
#include <service/attestation/claims/sources/event_log/mock/mock_event_log.h>
#include <psa/crypto.h>


uint16_t own_id = 0; /* !!Needs refactoring as parameter to ffarpc_caller_init */


static int sp_init(uint16_t *own_sp_id);

void __noreturn sp_main(struct ffa_init_info *init_info)
{
	/* Service provider objects */
	struct attest_provider attest_provider;
	struct rpc_interface *attest_iface;
	struct ffa_call_ep ffarpc_call_ep;
	struct ffa_direct_msg req_msg;

	/* Claim source objects */
	struct claim_source *claim_source;
	struct event_log_claim_source event_log_claim_source;
    struct boot_seed_generator boot_seed_claim_source;
    struct null_lifecycle_claim_source lifecycle_claim_source;
    struct instance_id_claim_source instance_id_claim_source;

	/*********************************************************
	 * Boot phase
	 *********************************************************/
	if (sp_init(&own_id) != 0) goto fatal_error;

	config_ramstore_init();
	sp_config_load(init_info);

	/**
	 * Register claim sources for deployment
     */
	claims_register_init();

    /* Boot measurement claim source - uses mock event log */
    claim_source = event_log_claim_source_init(&event_log_claim_source,
        mock_event_log_start(), mock_event_log_size());
    claims_register_add_claim_source(CLAIM_CATEGORY_BOOT_MEASUREMENT, claim_source);

    /* Boot seed claim source */
    claim_source = boot_seed_generator_init(&boot_seed_claim_source);
    claims_register_add_claim_source(CLAIM_CATEGORY_DEVICE, claim_source);

    /* Lifecycle state claim source */
    claim_source = null_lifecycle_claim_source_init(&lifecycle_claim_source);
    claims_register_add_claim_source(CLAIM_CATEGORY_DEVICE, claim_source);

    /* Instance ID claim source */
    claim_source = instance_id_claim_source_init(&instance_id_claim_source);
    claims_register_add_claim_source(CLAIM_CATEGORY_DEVICE, claim_source);

	/**
	 * Initialize the service provider
     */
 	psa_crypto_init(); /* temporary */

    attest_iface = attest_provider_init(&attest_provider, ATTEST_KEY_MNGR_VOLATILE_IAK);

    attest_provider_register_serializer(&attest_provider,
        TS_RPC_ENCODING_PACKED_C, packedc_attest_provider_serializer_instance());

	ffa_call_ep_init(&ffarpc_call_ep, attest_iface);

	/*********************************************************
	 * End of boot phase
	 *********************************************************/
	ffa_msg_wait(&req_msg);

	while (1) {
		if (req_msg.function_id == FFA_MSG_SEND_DIRECT_REQ_32) {

			struct ffa_direct_msg resp_msg;

			ffa_call_ep_receive(&ffarpc_call_ep, &req_msg, &resp_msg);

			ffa_msg_send_direct_resp(req_msg.destination_id,
					req_msg.source_id, resp_msg.args[0], resp_msg.args[1],
					resp_msg.args[2], resp_msg.args[3], resp_msg.args[4],
					&req_msg);
		}
	}

fatal_error:
	/* SP is not viable */
	EMSG("Attestation SP error");
	while (1) {}
}

void sp_interrupt_handler(uint32_t interrupt_id)
{
	(void)interrupt_id;
}

static int sp_init(uint16_t *own_sp_id)
{
	int status = -1;
	ffa_result ffa_res;
	sp_result sp_res;
	static uint8_t tx_buffer[4096] __aligned(4096);
	static uint8_t rx_buffer[4096] __aligned(4096);

	sp_res = sp_rxtx_buffer_map(tx_buffer, rx_buffer, sizeof(rx_buffer));
	if (sp_res == SP_RESULT_OK) {
		ffa_res = ffa_id_get(own_sp_id);
		if (ffa_res == FFA_OK) {
			status = 0;
		}
	}

	return status;
}