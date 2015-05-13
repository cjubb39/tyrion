#include "blargen.h"

void blargen::input(void) {
	data_in.reset_get();
	done_input.write(false);
	wait();

	for (int i = 0; i < SIZE; ++i) {
		data[i] = data_in.get();
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


	done_process.write(true);

	do {wait();}
	while(true);
}

void blargen::output(void) {
	data_out.reset_put();
	wait();

	do {wait();}
	while(!done_process.read());

	for (int i = 0; i < SIZE; ++i) {
		data_out.put(data[i]);
		wait();
	}

	do {wait();}
	while(true);
}

#ifdef __CTOS__
SC_MODULE_EXPORT(blargen)
#endif
