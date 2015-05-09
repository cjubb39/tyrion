#ifndef _TYRION_H_
#define _TYRION_H_

#include <linux/ioctl.h>

typedef struct {
	uint32_t length;
	uint64_t *mem_base;
} tyrion_arg_t;

#define TYRION_MAGIC 'q'

/* ioctls and their arguments */
#define TYRION_READ  _IOW(TYRION_MAGIC, 0, tyrion_arg_t *)
#define TYRION_WRITE _IOW(TYRION_MAGIC, 1, tyrion_arg_t *)

#endif
