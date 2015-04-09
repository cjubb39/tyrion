#ifndef SVD_SYNC_H
#define SVD_SYNC_H

#include "device.h" 

enum svd_regs {
	SVD_REG_CMD,          /* Command and status;
                           0x1 to start computation; 0x0 to reset */
	SVD_REG_SRC,
	SVD_REG_DST,
	SVD_REG_SIZE,         /* Number of elements per sample */
	SVD_REG_MAX_SIZE,     /* READ-ONLY max allowed size */
	SVD_REG_ID,           /* Device ID from OS */
	SVD_NR_REGS,
};

/* status bits in SVD_REG_CMD */
#define STATUS_DONE BIT(5);
#define STATUS_RUN  BIT(4);

struct svd_sync {
	struct device dev;
};

void svd_main(struct device *dev);

static inline struct svd_sync *dev_to_svd(struct device *dev) {
	return container_of(dev, struct svd_sync, dev);
}

static inline struct svd_sync *obj_to_svd(struct object *obj) {
	struct device *dev = obj_to_device(obj);

	return dev_to_svd(dev);
}

#endif /* SVD_SYNC_H */
