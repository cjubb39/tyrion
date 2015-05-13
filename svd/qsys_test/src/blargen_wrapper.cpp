#include "blargen_wrapper.h"

void blargen_wrapper::handle_input() {
	unsigned tmp;

	data_in.reset_get();
	data_to_dut.reset_put();
	wait();

	while (true) {
WRAPPER_INPUT_LOOP:
			tmp = data_in.get();
			wait();

			data_to_dut.put(tmp);
			wait();
	}
}

void blargen_wrapper::handle_output() {
	unsigned tmp;

	data_out.reset_put();
	data_from_dut.reset_get();
	wait();

	while (true) {
WRAPPER_OUTPUT_LOOP:
			tmp = data_from_dut.get();
			wait();

			data_out.put(tmp);
			wait();
	}
}
#ifdef __CTOS__
SC_MODULE_EXPORT(blargen_wrapper)
#endif
