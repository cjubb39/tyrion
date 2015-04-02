#include "svd.h"
#include "svd_tb.h"
#include "mydata.h"

int sc_main(int, char**) {
	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated",
			SC_DO_NOTHING);

	int clk_in = 10;

	sc_clock clk("clk", clk_in, SC_NS);
	sc_signal<bool> rst;
	sc_signal<bool> rst_dut;
	
	/* from TB to DUT */
	sc_signal<bool> rd_request;
	sc_signal<bool> rd_grant;
	sc_signal<svd_token_t> rd_data;

	/* from DUT to TB */
	sc_signal<bool> wr_request;
	sc_signal<bool> wr_grant;
	sc_signal<svd_token_t> wr_data;

	svd dut("dut");
	svd_tb tb("tb");

	dut.clk(clk);
	dut.rst(rst_dut);
	dut.data_in_req(rd_request);
	dut.data_in_grant(rd_grant);
	dut.data_in(rd_data);
	dut.data_out_req(wr_request);
	dut.data_out_grant(wr_grant);
	dut.data_out(wr_data);

	tb.clk(clk);
	tb.rst(rst);
	tb.rst_dut(rst_dut);
	tb.data_in(wr_data);
	tb.req_in(wr_request);
	tb.grant_in(wr_grant);
	tb.data_out(rd_data);
	tb.req_out(rd_request);
	tb.grant_out(rd_grant);


	/* simulation */
	rst.write(false);
	sc_start(clk_in*2, SC_NS);
	rst.write(true);

	sc_start();
}
