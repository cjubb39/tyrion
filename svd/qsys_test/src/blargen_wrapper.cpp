#include "blargen_wrapper.h"

void blargen_wrapper::handle_reset() {
	dut_rst.write(false);
	wait();
	dut_rst.write(true);

	while (true) {
		do {wait();}
		while(!dut_done.read());

		dut_rst.write(false);
		wait();
		dut_rst.write(true);
	}
}
	

void blargen_wrapper::handle_input() {
	unsigned tmp, tmp2, res;

	data_in.reset_get();
	data_to_dut.reset_put();
	wait();

	while (true) {
WRAPPER_INPUT_LOOP:
			tmp = data_in.get();
			wait();

			tmp2 = data_in.get();
			wait();

			res = tmp + tmp2 % 2;

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

			data_out.put(tmp);
			wait();
	}
}
#ifdef __CTOS__
SC_MODULE_EXPORT(blargen_wrapper)
#endif
