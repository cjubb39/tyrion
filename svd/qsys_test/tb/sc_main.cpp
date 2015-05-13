#include "blargen.h"
#include "blargen_wrapper.h"
#include "blargen_tb.h"

int sc_main(int, char **) {
	int clk_in = 10;
	int rst_in = 2*clk_in;

	sc_clock clk("clk", clk_in, SC_PS);
	sc_signal<bool> rst("rst"); 

	put_get_channel<unsigned> din;
	put_get_channel<unsigned> dout;

	blargen dut("dut");
	blargen_tb tb("tb");


	dut.clk(clk);
	dut.rst(rst);
	dut.data_in(din);
	dut.data_out(dout);

	tb.clk(clk);
	tb.rst(rst);
	tb.data_to_dut(din);
	tb.data_from_dut(dout);


	rst.write(false);
	sc_start(rst_in, SC_PS);
	rst.write(true);

	sc_start();
	
	return 0;
}
