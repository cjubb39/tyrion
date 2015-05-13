#include "blargen_wrapper.h"

void blargen_wrapper::handle_input() {
	unsigned tmp;
	unsigned lower_half, upper_half;

	data_in.reset_get();
	data_to_dut.reset_put();
	wait();

	while (true) {
WRAPPER_INPUT_LOOP:
			lower_half = data_in.get();
			wait();
#if 0
			upper_half = data_in.get();
			wait();
#endif

			tmp = lower_half + upper_half;
			data_to_dut.put(tmp);
			wait();
	}
}

void blargen_wrapper::handle_output() {
	unsigned tmp;
	unsigned lower_half, upper_half;

	data_out.reset_put();
	data_from_dut.reset_get();
	wait();

	while (true) {
WRAPPER_OUTPUT_LOOP:
			tmp = data_from_dut.get();
			wait();

			lower_half = tmp;
			upper_half = tmp;

#if 0
			data_out.put(lower_half);
			wait();
#endif

			data_out.put(upper_half);
			wait();
	}
}
#ifdef __CTOS__
SC_MODULE_EXPORT(blargen_wrapper)
#endif
