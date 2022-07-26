// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
 */

#include <common/fdt/fdt_helpers.h>
#include <config/interface/config_store.h>
#include <config/interface/config_blob.h>
#include <platform/interface/device_region.h>
#include <platform/interface/memory_region.h>
#include <stdbool.h>
#include <string.h>
#include <trace.h>

#include "sp_config_loader.h"

/*
 * According to the FF-A 1.0 spec: in the SP manifest the size of device and
 * memory regions is expressed as a count of 4K pages.
 */
#define FFA_SP_MANIFEST_PAGE_SIZE	0x1000

struct sp_param_region {
	char name[16];
	uintptr_t location;
	size_t size;
};

static void load_device_regions(const struct ffa_name_value_pair *value_pair);
static void load_memory_regions(const struct ffa_name_value_pair *value_pair);
static void load_blob(const struct ffa_name_value_pair *value_pair);
static bool load_fdt(const struct ffa_name_value_pair *value_pair);

/**
 * Loads externally provided configuration data passed into the SP via
 * FFA initialisation parameters.  Data can originate from
 * the SP manifest, an external device tree or a dynamic configuration
 * mechanism such as a handover block (HOB).
 */
void sp_config_load(struct ffa_init_info *init_info)
{
	/* Load deployment specific configuration */
	for (size_t param_index = 0; param_index < init_info->count; param_index++) {

   		if (!strcmp((const char *)init_info->nvp[param_index].name,"DEVICE_REGIONS")) {

			load_device_regions(&init_info->nvp[param_index]);
		}
		else if (!strcmp((const char *)init_info->nvp[param_index].name,"MEMORY_REGIONS")) {

			load_memory_regions(&init_info->nvp[param_index]);
		}
		else if (!memcmp(init_info->nvp[param_index].name,"TYPE_DT\0\0\0\0\0\0\0\0",
				 sizeof(init_info->nvp[param_index].name))) {
			if (!load_fdt(&init_info->nvp[param_index]))
				EMSG("Failed to load SP config from DT");
		}
		else {

			load_blob(&init_info->nvp[param_index]);
		}
	}
}

static void load_device_regions(const struct ffa_name_value_pair *value_pair)
{
	struct sp_param_region *d = (struct sp_param_region *)value_pair->value;

	/* Iterate over the device regions */
	while ((uintptr_t)d < (value_pair->value + value_pair->size)) {

		struct device_region device_region;

		strcpy(device_region.dev_class, d->name);
		device_region.dev_instance = 0;
		device_region.base_addr = d->location;
		device_region.io_region_size = d->size;

		config_store_add(CONFIG_CLASSIFIER_DEVICE_REGION,
			device_region.dev_class, device_region.dev_instance,
			&device_region, sizeof(device_region));

		++d;
	}
}

static void load_memory_regions(const struct ffa_name_value_pair *value_pair)
{
	struct sp_param_region *d = (struct sp_param_region *)value_pair->value;

	/* Iterate over the device regions */
	while ((uintptr_t)d < (value_pair->value + value_pair->size)) {

		struct memory_region memory_region;

		strcpy(memory_region.region_name, d->name);
		memory_region.base_addr = d->location;
		memory_region.region_size = d->size;

		config_store_add(CONFIG_CLASSIFIER_MEMORY_REGION,
			memory_region.region_name, 0,
			&memory_region, sizeof(memory_region));

		++d;
	}
}

static void load_blob(const struct ffa_name_value_pair *value_pair)
{
	struct config_blob blob;

	blob.data = (const void*)value_pair->value;
	blob.data_len = value_pair->size;

	config_store_add(CONFIG_CLASSIFIER_BLOB,
		(const char *)value_pair->name, 0,
		&blob, sizeof(blob));
}

static bool load_fdt(const struct ffa_name_value_pair *value_pair)
{
	const void *fdt = (const void *)value_pair->value;
	size_t fdt_size = value_pair->size;
	int root = -1, node = -1, subnode = -1, rc = -1;

	/* Sanity check */
	if (!fdt) {
		DMSG("error: fdt NULL pointer");
		return false;
	}

	rc = fdt_check_full(fdt, fdt_size);
	if (rc) {
		DMSG("error: fdt_check_full(): %d", rc);
		return false;
	}

	/* Find root node */
	root = fdt_path_offset(fdt, "/");
	if (root < 0) {
		DMSG("error: fdt_path_offset(): %d", root);
		return false;
	}

	/* Check if it's a valid SP manifest */
	rc = fdt_node_check_compatible(fdt, root, "arm,ffa-manifest-1.0");
	if (rc) {
		DMSG("error: fdt_node_check_compatible(): %d", rc);
		return false;
	}

	/* Find memory regions */
	node = fdt_node_offset_by_compatible(fdt, root, "arm,ffa-manifest-memory-regions");
	if (node >= 0) {
		fdt_for_each_subnode(subnode, fdt, node) {
			struct memory_region memory_region = {0};
			uint64_t base_addr = 0;
			uint32_t page_cnt = 0;
			const char *subnode_name = fdt_get_name(fdt, subnode, NULL);

			if (!subnode_name) {
				DMSG("error: subnode name is missing");
				return false;
			}

			if(!dt_get_u64(fdt, subnode, "base-address", &base_addr)) {
				DMSG("error: base-address is missing");
				return false;
			}

			if(!dt_get_u32(fdt, subnode, "pages-count", &page_cnt)) {
				DMSG("error: pages-count is missing");
				return false;
			}

			strncpy(memory_region.region_name, subnode_name,
				sizeof(memory_region.region_name));
			memory_region.base_addr = (uintptr_t)base_addr;
			memory_region.region_size = page_cnt * FFA_SP_MANIFEST_PAGE_SIZE;

			if (!config_store_add(CONFIG_CLASSIFIER_MEMORY_REGION,
					      memory_region.region_name, 0,
					      &memory_region, sizeof(memory_region))) {
				DMSG("error: failed to add memory region to config store");
				return false;
			}
		}
	}

	/* Find device regions */
	node = fdt_node_offset_by_compatible(fdt, root, "arm,ffa-manifest-device-regions");
	if (node >= 0) {
		fdt_for_each_subnode(subnode, fdt, node) {
			struct device_region device_region = {0};
			uint64_t base_addr = 0;
			uint32_t page_cnt = 0;
			const char *subnode_name = fdt_get_name(fdt, subnode, NULL);

			if (!subnode_name) {
				DMSG("error: subnode name is missing");
				return false;
			}

			if(!dt_get_u64(fdt, subnode, "base-address", &base_addr)) {
				DMSG("error: base-address is missing");
				return false;
			}

			if (!dt_get_u32(fdt, subnode, "pages-count", &page_cnt)) {
				DMSG("error: pages-count is missing");
				return false;
			}

			strncpy(device_region.dev_class, subnode_name,
				sizeof(device_region.dev_class));
			device_region.base_addr = base_addr;
			device_region.io_region_size = page_cnt * FFA_SP_MANIFEST_PAGE_SIZE;
			device_region.dev_instance = 0;

			if (!config_store_add(CONFIG_CLASSIFIER_DEVICE_REGION,
					      device_region.dev_class, device_region.dev_instance,
					      &device_region, sizeof(device_region))) {
				DMSG("error: failed to add device region to config store");
				return false;
			}
		}
	}

	/* Find TPM event log */
	node = fdt_node_offset_by_compatible(fdt, root, "arm,tpm_event_log");
	if (node >= 0) {
		uint64_t tpm_event_log_addr = 0;
		uint32_t tpm_event_log_size = 0;

		if (!dt_get_u64(fdt, node, "tpm_event_log_addr", &tpm_event_log_addr)) {
			DMSG("error: tpm_event_log_addr is missing");
			return false;
		}

		if (!dt_get_u32(fdt, node, "tpm_event_log_size", &tpm_event_log_size)) {
			DMSG("error: tpm_event_log_size is missing");
			return false;
		}

		if (!config_store_add(CONFIG_CLASSIFIER_BLOB, "EVENT_LOG", 0,
				      (void *)tpm_event_log_addr, tpm_event_log_size)) {
			DMSG("error: failed to add event log to config store");
			return false;
		}
	}

	return true;
}
