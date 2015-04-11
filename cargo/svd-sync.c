#include "device-list.h"
#include "alloc.h"

#include "svd-sync.h"

static void svd_release(struct object *obj) {
	struct svd_sync *svd = obj_to_svd(obj);

	free(svd);
}

static int svd_create(const struct device_desc *desc, const char *name) {
	struct svd_sync *svd;

	svd = cargo_zalloc(sizeof(*svd));
	if (svd == NULL) {
		return -1;
	}

	svd->dev.obj.release = svd_release;
	svd->dev.obj.name = name;
	svd->dev.id = desc->id;
	svd->dev.length = SVD_NR_REGS * sizeof(u32);

	if (device_sync_register(&svd->dev, svd_main)) {
		free(svd);
		return -1;
	}

	return 0;
}

const struct device_init_operations svd_sync_init_ops = {
	.create = svd_create,
};

