/*
 * svd-mmap.c
 * Sowftware and hardware-accelerated SVD.
 *
 */
#include <sys/mman.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>

#include "svd.h"
#include "svd_data.h"

//#define SVD_OUTPUT_LENGTH(__sz) (SVD_OUTPUT_SIZE(__sz) / sizeof(SVD_CELL_TYPE))

static const char *devname = "/dev/svd.0";

#if 0
int main(int argc, char *argv[]) {
	u16 *buf;
	struct svd_access desc;
	int rc;

	input  = malloc(sizeof(*input) * SVD_INPUT_SIZE(sz));
	result = malloc(sizeof(*result) * SVD_OUTPUT_SIZE(sz));
	golden = malloc(sizeof(*golden) * SVD_OUTPUT_SIZE(sz));

	fd = open(devname, O_RDWR, 0);
	buf = mmap(NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memmove(buf, input, SVD_INPUT_SIZE(sz));
	desc.size = sz;
	rc = ioctl(fd, SVD_IOC_ACCESS, &desc);
	memmove(result, buf + SVD_INPUT_SIZE(sz)/ sizeof(u16), SVD_OUTPUT_SIZE(sz));
}
#endif
int main(int argc, char **argv) {
	size_t sz = 3, buf_size;
	char *buf;

	int fd, rc;
	struct svd_access desc;

	SVD_CELL_TYPE *input_matrix;
	SVD_CELL_TYPE *output_matrix;

	input_matrix = malloc(sizeof(*input_matrix) * SVD_INPUT_SIZE(sz));
	output_matrix = malloc(sizeof(*output_matrix) * SVD_OUTPUT_SIZE(sz));

	fd = open(devname, O_RDWR, 0);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	buf_size = SVD_BUF_SIZE(sz);

	buf = mmap(NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	memmove(buf, input_matrix, SVD_INPUT_SIZE_BYTE(sz));

	desc.size = sz;

#if 0
	rc = ioctl(fd, SVD_IOC_ACCESS, &desc);
	if (rc < 0) {
		perror("ioctl");
		exit(EXIT_FAILURE);
	}
#endif

	memmove(output_matrix, buf + SVD_INPUT_SIZE_BYTE(sz),
			SVD_OUTPUT_SIZE_BYTE(sz));

	free(input_matrix);
	free(output_matrix);
	close(fd);

	return 0;
}
