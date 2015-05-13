#ifndef _SVD_WRAPPER_H_
#define _SVD_WRAPPER_H_

#define SC_INCLUDE_FX
#include "systemc.h"
#include <ctos_flex_channels.h>

#include <stdint.h>

#include "svd.h"
#include "svd_data.h"

SC_MODULE(svd_wrapper) {
	sc_in<bool> clk;
	sc_in<bool> rst;

	get_initiator<uint32_t> data_in;
	put_initiator<uint32_t> data_out;

	sc_out<bool> svd_done_irq;

	void config_svd(void);
	void handle_irq(void);
	void handle_input(void);
	void handle_output(void);

	SC_CTOR(svd_wrapper):
		dut("svd")
 	{
		/* processes */
		SC_CTHREAD(config_svd, clk.pos());
		reset_signal_is(rst, false);
		SC_CTHREAD(handle_irq, clk.pos());
		reset_signal_is(rst, false);
		SC_CTHREAD(handle_input, clk.pos());
		reset_signal_is(rst, false);
		SC_CTHREAD(handle_output, clk.pos());
		reset_signal_is(rst, false);

		/* external interface */
		data_in.clk_rst(clk, rst);
		data_out.clk_rst(clk, rst);

		/* this -> signals */
		data_to_dut(bufdin);
		data_from_dut(bufdout);
		data_to_dut.clk_rst(clk, rst);
		data_from_dut.clk_rst(clk, rst);

		/* signals -> dut */
		dut.clk(clk);
		dut.rst(dut_rst);
		dut.rd_index(rd_index);
		dut.rd_length(rd_length);
		dut.rd_request(rd_request);
		dut.rd_grant(rd_grant);
		dut.wr_index(wr_index);
		dut.wr_length(wr_length);
		dut.wr_request(wr_request);
		dut.wr_grant(wr_grant);
		dut.bufdin(bufdin);
		dut.bufdout(bufdout);
		dut.conf_size(conf_size);
		dut.conf_done(conf_done);
		dut.svd_done(svd_done);
	}

private:
	svd dut;
	sc_signal<bool> dut_rst;
	sc_signal<unsigned> rd_index;
	sc_signal<unsigned> rd_length;
	sc_signal<bool> rd_request;
	sc_signal<bool> rd_grant;
	sc_signal<unsigned> wr_index;
	sc_signal<unsigned> wr_length;
	sc_signal<bool> wr_request;
	sc_signal<bool> wr_grant;
	put_get_channel<SVD_CELL_TYPE> bufdin;
	put_get_channel<SVD_CELL_TYPE> bufdout;
	sc_signal<unsigned> conf_size;
	sc_signal<bool> conf_done;
	sc_signal<bool> svd_done;

	put_initiator<SVD_CELL_TYPE> data_to_dut;
	get_initiator<SVD_CELL_TYPE> data_from_dut;
};


#endif
