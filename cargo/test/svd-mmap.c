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
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "svd_driver.h"

static const char *devname = "/dev/svd.0";

int main(int argc, char *argv[])
{
	unsigned flag;
	void *buf;
	int fd;
	size_t buf_size;
	size_t sample_size;

	struct svd_access desc;
	int rc;

	printf("Open: %s\n", devname);
	fd = open(devname, O_RDWR, 0);
	if (fd < 0)
	{
		perror("open");
		exit(1);
	}

	buf_size = SVD_BUF_SIZE;
	sample_size = SVD_OUTPUT_SIZE;

	/* main memory */
	buf = mmap(NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED)
	{
		perror("mmap");
		exit(1);
	}

	/* load data - copy data from input data structures*/
	// for (i = 0; i < num; i++) {
	// memcpy(buf, bayer, sizeof(bayer));
	// printf("Size of bayer: %lu\n", sizeof(bayer));
	// 	memcpy(buf + ((2 * i + 1) * sz), coeff + (i * sz), sample_size);
	// }

	// printf("\nStart: SVD as HARDWARE.\n");
	desc.cmd = SVD_CMD_RESET;
	// // desc.num_samples = num;

	// Configure the device and run it
	rc = ioctl(fd, SVD_IOC_ACCESS, &desc);
	if (rc < 0)
	{
		perror("ioctl");
		exit(1);
	}


	clock_t t;
	double time_spent;

	t = clock();
	printf("Current clicks: %d\n", t);
	/* here, do your time-consuming job */
	// Retrieve the task
	svd_task_t hardware_task;
	int i, itr = 1<<18;
	for (i = 0; i < itr; i++)
		memcpy(&hardware_task, ((char *) buf) + SVD_INPUT_SIZE, sizeof(hardware_task));
	t = clock() - t;
	time_spent = ((double) t) / CLOCKS_PER_SEC;
	printf("Task info: pid: %u, pri: %u, state: %u, time_spent: %f, clicks: %d\n", hardware_task.pid, hardware_task.pri, hardware_task.state, time_spent, t);

	if (munmap(buf, buf_size)) {
		perror("munmap");
		exit(1);
	}
	close(fd);

	return 0;
}
