/*
 * Copyright (c) 2020, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <service_locator.h>
#include <service/locator/linux/ffa/linuxffa_location_strategy.h>


void service_locator_envinit(void)
{
    /*
     * Register all service location strategies that could be used
     * to locate services from Linux userspace.
     */
    service_locator_register_strategy(linuxffa_location_strategy());
}