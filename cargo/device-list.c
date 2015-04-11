#include <string.h>
#include <errno.h>

#include <ccan/array_size/array_size.h>

#include "device-list.h"
#include "device.h"
#include "exit.h"

extern const struct device_init_operations svd_sync_init_ops;

static const struct device_desc device_list[] = {
	{
		.type		= "svd-sync",
		.description	= "SystemC SVD",
		.ops		= &svd_sync_init_ops,
		.id		= 0x2,
	},
};


const struct device_desc *device_desc_by_type(const char *type)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(device_list); i++) {
		const struct device_desc *desc = &device_list[i];

		if (!strcmp(type, desc->type))
			return desc;
	}
	errno = ENODEV;
	return NULL;
}
