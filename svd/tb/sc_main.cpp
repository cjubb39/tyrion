#define SC_INCLUDE_FX
#include "systemc.h"
#include <ctos_flex_channels.h>

#include "svd.h"
#include "svd_tb.h"
#include "svd_data.h"

int sc_main(int, char**) {
	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated",
			SC_DO_NOTHING);

	int clk_in = 10;

	sc_clock clk("clk", clk_in, SC_NS);
	sc_signal<bool> rst;
	sc_signal<bool> rst_dut;
	
#if 0
	/* from TB to DUT */
	sc_signal<bool> rd_request;
	sc_signal<bool> rd_grant;
	sc_signal<svd_token> rd_data;

	/* from DUT to TB */
	sc_signal<bool> wr_request;
	sc_signal<bool> wr_grant;
	sc_signal<svd_token> wr_data;
#endif

	// DMA requests interface from memory to device
	sc_signal<unsigned> rd_index;   // array index (offset from base address)
	sc_signal<unsigned> rd_length;  // burst size
	sc_signal<bool>     rd_request; // transaction request
	sc_signal<bool>      rd_grant;   // transaction grant

	// DMA requests signalterface from device to memory
	sc_signal<unsigned> wr_index;   // array index (offset from base address)
	sc_signal<unsigned> wr_length;  // burst size
	sc_signal<bool>     wr_request; // transaction request
	sc_signal<bool>      wr_grant;   // transaction grant

	// input data read by load_input
	put_get_channel<SVD_CELL_TYPE> bufdin("bufdin-yolo");
	// output data written by store output
	put_get_channel<SVD_CELL_TYPE> bufdout("bufdout-yolo");

	sc_signal<unsigned> conf_size;
	sc_signal<bool>     conf_done;

	// computation complete
	sc_signal<bool>    svd_done;


	svd dut("dut");
	svd_tb tb("tb");

	dut.clk(clk);
	dut.rst(rst_dut);
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

	tb.clk(clk);
	tb.rst(rst);
	tb.rst_dut(rst_dut);
#if 0
	tb.data_in(rd_data);
	tb.req_in(rd_request);
	tb.grant_in(rd_grant);
	tb.data_out(wr_data);
	tb.req_out(wr_request);
	tb.grant_out(wr_grant);
#endif
	tb.rd_index(rd_index);
	tb.rd_length(rd_length);
	tb.rd_request(rd_request);
	tb.rd_grant(rd_grant);
	tb.wr_index(wr_index);
	tb.wr_length(wr_length);
	tb.wr_request(wr_request);
	tb.wr_grant(wr_grant);
	tb.bufdin(bufdin);
	tb.bufdout(bufdout);
	tb.conf_size(conf_size);
	tb.conf_done(conf_done);
	tb.svd_done(svd_done);


	/* simulation */
	rst.write(false);
	sc_start(clk_in*2, SC_NS);
	rst.write(true);

	sc_start();
}
