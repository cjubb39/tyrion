#include "svd_wrapper.h"

void svd_wrapper::config_svd() {
	conf_done.write(false);
	wait();

	conf_size.write(MAX_SIZE);
	conf_done.write(true);

	do {wait();}
	while (true);
}

void svd_wrapper::handle_irq() {
	svd_done_irq.write(false);
	dut_rst.write(false);
	wait();
	dut_rst.write(true);

	do {wait();}
	while(true);
	/* TODO reset self properly */

	while (true) {
		do {wait();}
		while (!svd_done.read());

		/* TODO handle irq properly */
		/* XXX don't need this functionality in hardware */
		svd_done_irq.write(true);
		wait(); wait(); wait(); wait();
		wait(); wait(); wait(); wait();
		svd_done_irq.write(false);

		dut_rst.write(false);
		wait();
		dut_rst.write(true);
	}
}

void svd_wrapper::handle_input() {
	SVD_CELL_TYPE tmp;
	sc_uint<32> tmp_sc_uint;
	uint64_t whole;
	uint32_t lower_half, upper_half;
	unsigned length, index;

	data_in.reset_get();
	data_to_dut.reset_put();
	wait();

	while (true) {
		do { wait(); }
		while (!rd_request.read());
		rd_grant.write(true);
		length = rd_length.read();
		index = rd_index.read();
		do { wait(); }
		while (rd_request.read());
		rd_grant.write(false);

WRAPPER_INPUT_LOOP:
		for (int i = 0; i < length; ++i) {
			tmp_sc_uint = data_in.get();
			lower_half = tmp_sc_uint;
			wait();

			tmp_sc_uint = data_in.get();
			upper_half = tmp_sc_uint;
			wait();

			whole = ((uint64_t) upper_half) << 32 + lower_half;
			CTOS_FX_ASSIGN_RANGE(tmp, whole);

			data_to_dut.put(tmp);
			wait();
		}
	}
}

void svd_wrapper::handle_output() {
	SVD_CELL_TYPE tmp;
	sc_uint<64> tmp_sc_uint;
	uint64_t whole;
	uint32_t lower_half, upper_half;
	unsigned length, index;

	data_out.reset_put();
	data_from_dut.reset_get();
	wait();

	while (true) {
		do { wait(); }
		while (!wr_request.read());
		wr_grant.write(true);
		length = wr_length.read();
		index = wr_index.read();
		do { wait(); }
		while (wr_request.read());
		wr_grant.write(false);

WRAPPER_OUTPUT_LOOP:
		for (int i = 0; i < length; ++i) {
			tmp = data_from_dut.get();

			/* TODO check this does what I expect */
			tmp_sc_uint = tmp.range();
			whole = tmp_sc_uint;

			lower_half = (uint32_t) whole;
			upper_half = (uint32_t) (whole >> 32);

			data_out.put(lower_half);
			wait();
			data_out.put(upper_half);
			wait();
		}
	}
}
#ifdef __CTOS__
SC_MODULE_EXPORT(svd_wrapper)
#endif
