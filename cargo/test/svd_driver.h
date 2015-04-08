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

#include "../svd_sched.h"

#define DRV_NAME  "svd"
#define PFX    DRV_NAME ": "
#define SVD_MAX_DEVICES  64

#define SVD_REG_CMD        0x00
#define SVD_REG_SRC        0x04
#define SVD_REG_DST        0x08
#define SVD_REG_MAX_SIZE   0x0c
#define SVD_REG_ID         0x10

#define SVD_CMD_IRQ_SHIFT  4
#define SVD_CMD_IRQ_MASK   0x3
#define SVD_CMD_IRQ_DONE   0x2

#define SVD_CMD_RESET		 0x1

#define SVD_SYNC_DEV_ID    0x2

/* Determine buffer size (bytes) */
#define SVD_INPUT_NUM 4
#define SVD_OUTPUT_NUM 4
#define SVD_INPUT_SIZE	(sizeof(svd_task_t) * SVD_INPUT_NUM)
#define SVD_OUTPUT_SIZE	(sizeof(svd_task_t) * SVD_OUTPUT_NUM)

#define SVD_BUF_SIZE	(SVD_INPUT_SIZE + SVD_OUTPUT_SIZE)

struct svd_access {
	uint32_t cmd;
};

#define SVD_IOC_ACCESS     _IOW ('I', 0, struct svd_access)
#define SVD_IOC_READ       _IOR ('I', 0, struct svd_access)

#endif /* _SVD_H_ */
