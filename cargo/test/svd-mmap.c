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
#include "gm_jacobi.h"

static const char *devname = "/dev/svd.0";

void compute_golden_model(SVD_CELL_TYPE *golden_input_matrix,
		SVD_CELL_TYPE *golden_matrix, int mat_size) {
	gm_jacobi(golden_input_matrix, mat_size,
			SVD_GET_S(golden_matrix, mat_size),
			SVD_GET_U(golden_matrix, mat_size),
			SVD_GET_V(golden_matrix, mat_size));
}

int main(int argc, char **argv) {
	size_t sz = 4, buf_size;
	char *buf;

	int fd, rc;
	struct svd_access desc;

	SVD_CELL_TYPE *input_matrix, *golden_input_matrix;
	SVD_CELL_TYPE *output_matrix, *golden_matrix;

	unsigned long mismatches = 0;
	size_t i, j;

	srand(time(NULL));

	input_matrix = malloc(sizeof(*input_matrix) * SVD_INPUT_SIZE(sz));
	golden_input_matrix = malloc(sizeof(*golden_input_matrix) * SVD_INPUT_SIZE(sz));
	output_matrix = malloc(sizeof(*output_matrix) * SVD_OUTPUT_SIZE(sz));
	golden_matrix = malloc(sizeof(*golden_matrix) * SVD_OUTPUT_SIZE(sz));

	for (i = 0; i < sz; ++i) {
		for (j = 0; j < sz; ++j) {
			input_matrix[i * sz + j] = rand();
		}
	}

	memcpy(golden_input_matrix, input_matrix, sizeof(*input_matrix) *
			SVD_INPUT_SIZE(sz));

	compute_golden_model(golden_input_matrix, golden_matrix, (int) sz);

	fd = open(devname, O_RDWR, 0);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	buf_size = SVD_BUF_SIZE_BYTE(sz);

	buf = mmap(NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	memmove(buf, input_matrix, SVD_INPUT_SIZE_BYTE(sz));

	desc.size = sz;

	rc = ioctl(fd, SVD_IOC_ACCESS, &desc);
	if (rc < 0) {
		perror("ioctl");
		exit(EXIT_FAILURE);
	}

	memmove(output_matrix, buf + SVD_INPUT_SIZE_BYTE(sz), SVD_OUTPUT_SIZE_BYTE(sz));

	for (i = 0; i < SVD_OUTPUT_SIZE(sz); ++i) {
		SVD_CELL_TYPE diff = output_matrix[i] - golden_matrix[i];
		if (diff < 0) {
			diff *= -1;
		}
		mismatches += !(diff < MAX_ERROR);
	}

	if (mismatches) {
		printf("Simulation failed with %lu errors\n", mismatches);
	} else {
		printf("Simulation success! No errors\n");
	}

	free(input_matrix);
	free(output_matrix);
	close(fd);

	return 0;
}
