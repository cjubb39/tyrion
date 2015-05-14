#include <stdio.h>
#include "fifo.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h> 

//#define CORRECTNESS_CHECK

int fifo0_fd;
int fifo1_fd;

#define MAX 8388608

void double_to_bv(double d, uint64_t* fixed) { 
	double sub = MAX; 
	*fixed = 0; 
	int i; 
	for (i = 63; i >= 0; i--) {
		if (d > sub) {
			//set the bit and substract the value from sub 
			d -= sub;
			*fixed |= ( 1UL << i); 
		}
		sub /= 2.0; 
	}
}

void convert_to_bv(double* fixed) {
	double d = *fixed; 
	double_to_bv(d, (uint64_t *) fixed); 
}

void bv_to_double(uint64_t fixed, double* d) { 
	double sub = MAX; 
	*d = 0.0; 
	int i; 

	for (i = 63; i >= 0; i--) {
		if ((fixed & (1UL << i))) {
			*d += sub;
		}
		sub /= 2.0; 
	}
}


void convert_to_double(uint64_t* d) {
	uint64_t fixed  = *d; 
	bv_to_double(fixed, (double*) d); 
}

#define SIZE (2*64*64)
void check_correct() {
	double* test = (double*) malloc(SIZE/2 * sizeof(double)); 

	uint64_t in, out;
	int input;  
	double res; 
	int j; 
	srand(NULL); //lol time on fpga board 
	for (j = 0; j < SIZE/2; j++) {
		test[j] = (double) rand() / 1234543.424;
	}
	for (j = 0; j < SIZE/2; j++) {
		double_to_bv(test[j], &in); 

		//should i give the high order bits first?
		input = (int) (in >> 32); 
		if (ioctl(fifo0_fd, VGA_LED_WRITE_DIGIT, &input)) {
			perror("ioctl(VGA_LED_WRITE_DIGIT) failed");
			return;
		}
		usleep(4000);
		if (j % 64 == 0)
			printf("%d inserting: %d\n",j,input); 

		input = (int) in; 
		if (ioctl(fifo0_fd, VGA_LED_WRITE_DIGIT, &input)) {
			perror("ioctl(VGA_LED_WRITE_DIGIT) failed");
			return;
		}
		usleep(4000);
		if (j % 64 == 0)
			printf("%d inserting: %d\n",j,input); 

	}
	printf("finished writing j: %d\n", j);
	sleep(2);
	printf("start reading\n");
	sleep(1);

	int res1, res2; 
	for(j = 0; j < 3 * SIZE/2; j++) {
		if (ioctl(fifo1_fd, VGA_LED_READ_DIGIT, &res1)) {
			perror("ioctl(VGA_LED_READ_DIGIT) failed");
			return;
		}
		usleep(4000);

		if (ioctl(fifo1_fd, VGA_LED_READ_DIGIT, &res2)) {
			perror("ioctl(VGA_LED_READ_DIGIT) failed");
			return;
		}
		usleep(4000);
		out = res1; 
		out <<= 32; 
		out += res2; //can /should i use an or here 

		bv_to_double(out, &res); 

		printf("Matrix %d: val %f bv %lu\n", j/(SIZE/2), res, out); 
	}
}


int main()
{
	int value_in,value_out;

	static const char filename0[] = "/dev/fifo0";
	static const char filename1[] = "/dev/fifo1";

	printf("FIFO Userspace program started\n");

	if ( (fifo0_fd = open(filename0, O_RDWR)) == -1) {
		fprintf(stderr, "could not open %s\n", filename0);
		return -1;
	}

	if ( (fifo1_fd = open(filename1, O_RDWR)) == -1) {
		fprintf(stderr, "could not open %s\n", filename1);
		return -1;
	}


	int i, j;
#ifndef CORRECTNESS_CHECK
	for(j = 0; j < SIZE / 2; ++j) {
		value_in = (2 * j) << 12;
		if (ioctl(fifo0_fd, VGA_LED_WRITE_DIGIT, &value_in)) {
			perror("ioctl(VGA_LED_WRITE_DIGIT) failed");
			return;
		}
		if (j % 64 == 0)
			printf("%d inserting: %d\n",j,value_in); 

		value_in = (2*j) << 8;
		if (ioctl(fifo0_fd, VGA_LED_WRITE_DIGIT, &value_in)) {
			perror("ioctl(VGA_LED_WRITE_DIGIT) failed");
			return;
		}
		if (j % 64 == 0)
			printf("%d inserting: %d\n",j,value_in); 
	}

	printf("finished writing\n");
	sleep(2);
	printf("start reading\n");
	sleep(1);

	for(j = 0; j < 3 * SIZE; ++j) {
		if (ioctl(fifo1_fd, VGA_LED_READ_DIGIT, &value_out)) {
			perror("ioctl(VGA_LED_READ_DIGIT) failed");
			return;
		}
		if (j % 64 == 0)
			printf("j: %d value_out: %d\n",j ,value_out); 
	}
#else
	check_correct(); 
#endif
	printf("FIFO Userspace program terminating\n");
	return 0;
}
