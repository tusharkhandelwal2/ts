/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
 */

#ifndef CALL_PARAM_BUF_COMPARATOR_H_
#define CALL_PARAM_BUF_COMPARATOR_H_

#include <CppUTestExt/MockSupport.h>
#include "../endpoint/rpc_interface.h"

class call_param_buf_comparator : public MockNamedValueComparator
{
public:
	enum check_mode {
		mode_normal = 0,
		mode_ignore_data_len
	};

	call_param_buf_comparator(check_mode mode = mode_normal) : mode(mode)
	{
	}

	virtual bool isEqual(const void *object1, const void *object2)
	{
		struct call_param_buf *buf1 = (struct call_param_buf *)object1;
		struct call_param_buf *buf2 = (struct call_param_buf *)object2;

		return (buf1->size == buf2->size) &&
			(mode == mode_ignore_data_len || (buf1->data_len == buf2->data_len)) &&
			(buf1->data == buf2->data);
	}

	// LCOV_EXCL_START
	virtual SimpleString valueToString(const void *object)
	{
		struct call_param_buf *buf = (struct call_param_buf *)object;

		return StringFromFormat("<size = %zu, data_len = %zu%s, data = %p>",
					buf->size, buf->data_len,
					(mode == mode_ignore_data_len) ? " (ignored)" : "",
					buf->data);
	}
	// LCOV_EXCL_STOP

private:
	check_mode mode;
};

#endif /* CALL_PARAM_BUF_COMPARATOR_H_ */
