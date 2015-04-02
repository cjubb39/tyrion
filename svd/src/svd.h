#ifndef _SVD_H_
#define _SVD_H_

#include "systemc.h"

#include "mydata.h"

SC_MODULE(svd) {
	sc_in<bool> clk;
	sc_in<bool> rst;

	sc_in<bool>        data_in_req;
	sc_out<bool>       data_in_grant;
	sc_in<svd_token> data_in;

	sc_out<bool>        data_out_req;
	sc_in<bool>         data_out_grant;
	sc_out<svd_token> data_out;

	void beh(void);

	SC_CTOR(svd) {
		SC_CTHREAD(beh, clk.pos());
		reset_signal_is(rst, false);
	}

	private:
	svd_token matrix_in;
	svd_token u, s, v;

};

#endif
