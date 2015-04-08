#ifndef __SVDTB_H__
#define __SVDTB_H__

#include "systemc.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <ostream>
#include "mydata.h"

SC_MODULE(svd_tb) {
	sc_in<bool> clk; 
	sc_in<bool> rst;
	sc_out<bool> rst_dut;

	sc_out<svd_token> data_in; 
	sc_out<bool> req_in; 
	sc_in<bool> grant_in; 

	sc_in<svd_token> data_out; 
	sc_in<bool> req_out; 
	sc_out<bool> grant_out; 

	void send(void);
	void recv(void);
	void fill_buf(void);

	SC_CTOR(svd_tb) 
		: clk("clk")
		, data_in("data_in")
		, req_in("req_in")
		, grant_in("grant_in")
		, data_out("data_out")
		, req_out("req_out")
		, grant_out("grant_out")
	{
		SC_CTHREAD(send, clk.pos()); 
		reset_signal_is(rst, false);
		SC_CTHREAD(recv, clk.pos()); 
		reset_signal_is(rst, false);
	}

	private:
	void compute_golden_model(void);
	int check_equality(svd_token&, svd_token&);
	svd_token gm_u;
	svd_token gm_s;
	svd_token gm_v;

	svd_token hw_u;
	svd_token hw_s;
	svd_token hw_v;

	svd_token svd_buf_in;
	svd_token svd_buf_out;


}; 
#endif 
