#ifndef SVD_SYNC_H
#define SVD_SYNC_H

#include "device.h"

// Registers: must match the list of registers in the driver
enum svd_regs {
  SVD_REG_CMD,         // Command and status register
                       // Write 0x1 to start computation
                       // Write 0x0 to reset the device
  SVD_REG_SRC,
  SVD_REG_DST,
  SVD_REG_MAX_SIZE,    // READ-ONLY maximum allowed size
  SVD_REG_ID,          // device ID assigned by OS.
  SVD_NR_REGS,
};

// Status bits in SVD_REG_CMD
#define STATUS_RUN  BIT(4); // svd running
                            // error if both set
#define SVD_CMD_RESET 0x1

struct svd_sync {
  struct device dev;
};

void svd_main(struct device *dev);

static inline struct svd_sync *dev_to_svd(struct device *device)
{
  return container_of(device, struct svd_sync, dev);
}

static inline struct svd_sync *obj_to_svd(struct object *object)
{
  struct device *dev = obj_to_device(object);

  return dev_to_svd(dev);
}

#endif /* SVD_SYNC_H */
