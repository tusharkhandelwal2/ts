// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2020, Arm Limited and Contributors. All rights reserved.
 */

#include <service/crypto/client/cpp/crypto_client.h>
#include <app/ts-demo/ts-demo.h>
#include <service_locator.h>
#include <rpc_caller.h>

int main(int argc, char *argv[]) {
	(void) argc;
	(void) argv;

	int status = -1;
	struct service_context *crypto_service_context = NULL;

	service_locator_init();

	crypto_service_context = service_locator_query("sn:trustedfirmware.org:crypto:0", &status);

	if (crypto_service_context) {

		struct rpc_caller *caller;
		rpc_session_handle rpc_session_handle;

		rpc_session_handle = service_context_open(crypto_service_context, &caller);

		if (rpc_session_handle) {

			crypto_client crypto_client(caller);

			status = run_ts_demo(&crypto_client, true);

			if (status != 0) {
				printf("run_ts_demo failed\n");
			}

			service_context_close(crypto_service_context, rpc_session_handle);
		}
		else {
			printf("Failed to open rpc session\n");
		}

		service_context_relinquish(crypto_service_context);
	}
	else {
		printf("Failed to discover crypto service\n");
	}

	return status;
}