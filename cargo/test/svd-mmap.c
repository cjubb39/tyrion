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

#define SVD_OUTPUT_LENGTH(__sz) (SVD_OUTPUT_SIZE(__sz) / sizeof(SVD_CELL_TYPE))

static const char *devname = "/dev/svd.0";

#if 0
static inline int validate(u16* golden, u16 *result, unsigned sz) {

}

static void init_golden(u16 *golden, unsigned sz) {
	FILE *gold = fopen("inout/golden_output.bin", "r");
	if (!gold) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	/* skip header and go */
	fseek(gold, 8, SEEK_SET);
	size_t res = fread(golden, sizeof(*golden), SVD_OUTPUT_LENGTH(sz), gold);
	if (res < SVD_OUTPUT_LENGTH(sz)) {
		perror("fread");
		exit(EXIT_FAILURE);
	}

	fclose(gold);
}

static void init_svd(u16 *input, unsigned sz) {
	FILE *in = fopen("inout/input.bin", "r");
	if (!in) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	/* skip header and go */
	fseek(in, 8, SEEK_SET);
	size_t res = fread(input, sizeof(*input), sz*sz, in);
	if (res < sz*sz) {
		perror("fread");
		exit(EXIT_FAILURE);
	}
	
	fclose(in);
}

int main(int argc, char *argv[]) {
	unsigned long long sw_ns, hw_ns;
	unsigned flag;
	u16 *buf;
	int fd;
	size_t buf_size;
	size_t sample_size;

	struct svd_access desc;
	int rc;

	int sz;

#ifdef SVD_SIZE
	sz = SVD_SIZE;
#else
	BUILD_BUG_ON_ZERO(1);
#endif

	/* set it all up */
	u16 *input;
	u16 *result;
	u16 *golden;

	input  = malloc(sizeof(*input) * SVD_INPUT_SIZE(sz));
	result = malloc(sizeof(*result) * SVD_OUTPUT_SIZE(sz));
	golden = malloc(sizeof(*golden) * SVD_OUTPUT_SIZE(sz));

	if (!input || !result || !golden) {
		fprintf(stderr, "malloc fail");
		exit(EXIT_FAILURE);
	}

	init_svd(input, sz);
	init_golden(golden, sz);

	printf("Debayer parameters:\n"
			   "\tsize = %d\n", sz);

	
#ifndef NO_SOFTWARE
	printf("\nStart: Debayer as SOFTWARE.\n");

	wami_svd(
			(rgb_pixel (*) [SVD_SIZE - 2 * WAMI_SVD_PAD])result,
			(u16 (*const) [SVD_SIZE]) input
		);

	printf("Done; Debayer as SOFTWARE.\n");
	
	flag = validate(
			golden,
			result,
			sz
		); 
	assert((flag == 0) && "Debayer SOFTWARE failed!\n");
	printf("SOFTWARE success!\n");
#endif

#ifndef NO_HARDWARE
	printf("\nOpen: %s\n", devname);
	fd = open(devname, O_RDWR, 0);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	buf_size = SVD_BUF_SIZE(sz);

	/* main memory */
	buf = mmap(NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	/* load input image */
	memmove(buf, input, SVD_INPUT_SIZE(sz));

	printf("Start: Debayer as HARDWARE.\n");
	desc.size = sz;

	rc = ioctl(fd, SVD_IOC_ACCESS, &desc);
	if (rc < 0) {
		perror("ioctl");
		exit(EXIT_FAILURE);
	}
	
	printf("Done; Debayer as HARDWARE.\n");

	/* did we do it right??? */	
	memmove(result, buf + SVD_INPUT_SIZE(sz)/ sizeof(u16), SVD_OUTPUT_SIZE(sz));
	flag = validate(
			golden,
			result,
			sz
		); 
	assert((flag == 0) && "Debayer HARDWARE failed!\n");
	printf("HARDWARE success!\n");
#endif

	free(input);
	free(result);
	free(golden);
	close(fd);

	return 0;
}
#endif
int main() {}
