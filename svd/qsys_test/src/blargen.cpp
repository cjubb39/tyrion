#include "blargen.h"

void blargen::input(void) {
	data_in.reset_get();
	done_input.write(false);
	wait();

	unsigned tmp;
	for (int i = 0; i < SIZE; ++i) {
		tmp = data_in.get();
		wait();
		d_in[i] = tmp;
		wait();
	}

	done_input.write(true);
	do {wait();}
	while(true);
}

void blargen::process(void) {
	done_process.write(false);
	wait();

	do {wait();}
	while(!done_input.read());

	int i;
	unsigned tmp, tmp2;
	for (i = 0; i < SIZE/2; ++i) {
		tmp = d_in[i];
		wait();
		tmp2 = d_in[SIZE-i];
		wait();
		d_out[i] = tmp2;
		wait();
		d_out[SIZE-i] = tmp;
		wait();
	}


	done_process.write(true);

	do {wait();}
	while(true);
}

void blargen::output(void) {
	data_out.reset_put();
	wait();

	do {wait();}
	while(!done_process.read());

	unsigned tmp;
	for (int i = 0; i < SIZE; ++i) {
		tmp = d_out[i];
		wait();
		data_out.put(tmp);
		wait();
	}

	do {wait();}
	while(true);
}

#ifdef __CTOS__
SC_MODULE_EXPORT(blargen)
#endif
