/*
 * Userspace program that communicates with the led_vga device driver
 * primarily through ioctls
 *
 * Stephen A. Edwards
 * Columbia University
 */

#include <stdio.h>
#include "fifo.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int fifo0_fd;
int fifo1_fd;

/* Read and print the segment values */
/* void print_segment_info() { */
/*   vga_led_arg_t vla; */
/*   int i; */

/*   for (i = 0 ; i < VGA_LED_DIGITS ; i++) { */
/*     vla.digit = i; */
/*     if (ioctl(vga_led_fd, VGA_LED_READ_DIGIT, &vla)) { */
/*       perror("ioctl(VGA_LED_READ_DIGIT) failed"); */
/*       return; */
/*     } */
/*     printf("%02x ", vla.segments); */
/*   } */
/*   printf("\n"); */
/* } */

/* Write the contents of the array to the display */
/* void write_segments(const unsigned char segs[8]) */
/* { */
/*   vga_led_arg_t vla; */
/*   int i; */
/*   for (i = 0 ; i < VGA_LED_DIGITS ; i++) { */
/*     vla.digit = i; */
/*     vla.segments = segs[i]; */
/*     if (ioctl(vga_led_fd, VGA_LED_WRITE_DIGIT, &vla)) { */
/*       perror("ioctl(VGA_LED_WRITE_DIGIT) failed"); */
/*       return; */
/*     } */
/*   } */
/* } */

int main()
{
	int value_in,value_out;

	static const char filename0[] = "/dev/fifo0";
	static const char filename1[] = "/dev/fifo1";

	/* static unsigned char message[8] = { 0x39, 0x6D, 0x79, 0x79, */
	/* 									0x66, 0x7F, 0x66, 0x3F }; */

	printf("FIFO Userspace program started\n");

	if ( (fifo0_fd = open(filename0, O_RDWR)) == -1) {
		fprintf(stderr, "could not open %s\n", filename0);
		return -1;
	}

	if ( (fifo1_fd = open(filename1, O_RDWR)) == -1) {
		fprintf(stderr, "could not open %s\n", filename1);
		return -1;
	}

#define SIZE (2*64*64)
	//(2*2*64*64)

	int i, j;
	//for (i = 0; i < 4; ++i) {
		for(j = 0; j < SIZE; ++j) {
		value_in = 0 * SIZE + j;
			if (ioctl(fifo0_fd, VGA_LED_WRITE_DIGIT, &value_in)) {
				perror("ioctl(VGA_LED_WRITE_DIGIT) failed");
				return;
			}
			usleep(4000);
			if (j % 64 == 0)
				printf("inserting: %d\n",value_in); 
		}
		usleep(4000);
		for(j = 0; j < 3 * SIZE; ++j) {
			if (ioctl(fifo1_fd, VGA_LED_READ_DIGIT, &value_out)) {
				perror("ioctl(VGA_LED_READ_DIGIT) failed");
				return;
			}
			usleep(4000);
			if (j % 64 == 0)
				printf("j: %d value_out: %d\n",j ,value_out); 
		}

	//}
	/* print_segment_info(); */

	/* write_segments(message); */

	/* printf("current state: "); */
	/* print_segment_info(); */

	/* for (i = 0 ; i < 24 ; i++) { */
	/*   unsigned char c0 = message[0]; */
	/*   memmove(message, message+1, VGA_LED_DIGITS - 1); */
	/*   message[VGA_LED_DIGITS - 1] = c0; */
	/*   write_segments(message); */
	/*   usleep(400000); */
	/* } */
  
	printf("FIFO Userspace program terminating\n");
	return 0;
}
