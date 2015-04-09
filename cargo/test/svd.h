#ifndef _SVD_H_
#define _SVD_H_

#ifdef __KERNEL__
#include <linux/ioctl.h>
#include <linux/types.h>
#else
#include <sys/ioctl.h>
#include <stdint.h>
#ifndef __user
#define __user
#endif
#endif /* __KERNEL__ */

#include "svd_data.h"

#define DRV_NAME  "svd"
#define PFX    DRV_NAME ": "
#define SVD_MAX_DEVICES  64

#define SVD_REG_CMD      0x00
#define SVD_REG_SRC      0x04
#define SVD_REG_DST      0x08
#define SVD_REG_SIZE     0x0C
#define SVD_REG_MAX_SIZE 0x10
#define SVD_REG_ID       0x14

#define SVD_CMD_IRQ_SHIFT    4
#define SVD_CMD_IRQ_MASK     0x03
#define SVD_CMD_IRQ_DONE     0x02

#define SVD_SYNC_DEV_ID      0x02

/* buffer sizes */
#define SVD_INPUT_SIZE_BYTE(__sz) (sizeof(SVD_CELL_TYPE) * __sz * __sz)
#define SVD_OUTPUT_SIZE_BYTE(__sz) (3 * sizeof(SVD_CELL_TYPE) * __sz * __sz)
#define SVD_BUF_SIZE(__sz) \
	(SVD_INPUT_SIZE_BYTE(__sz) + \
	 SVD_OUTPUT_SIZE_BYTE(__sz))

struct svd_access {
	unsigned size;
};

#define SVD_IOC_ACCESS  _IOW ('I', 0, struct svd_access)

#endif /* _SVD_H_ */
