/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
 */

#ifndef SP_HEADER_DEFINES_H
#define SP_HEADER_DEFINES_H

#define OPTEE_SP_UUID \
	{0x46bb39d1, 0xb4d9, 0x45b5, \
		{0x88, 0xff, 0x04, 0x00, 0x27, 0xda, 0xb2, 0x49}}

#define OPTEE_SP_FLAGS				0

/* Provisioned stack size */
#define OPTEE_SP_STACK_SIZE			(64 * 1024)

#endif /* SP_HEADER_DEFINES_H */
