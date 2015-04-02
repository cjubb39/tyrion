#include "systemc.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <ostream>
#include "mydata.h"

#ifndef __SVDTB_H__
#define __SVDTB_H__

#include "mydata.h"

SC_MODULE(svd_tb) {
    sc_in<bool> clk; 
    sc_in<bool> rst;
    sc_in<bool> rst_dut;

    sc_out<svd_token> data_in; 
    sc_out<bool> req_in; 
    sc_in<bool> grant_in; 

    sc_in<svd_token> data_out; 
    sc_in<bool> req_out; 
    sc_out<bool> grant_out; 

		void send(void);
		void recv(void);

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

#if 0
    void fill_buf() {
        //Not sure what headers I can use here so 
        //real basic matrix
        int i = 0; 
        for(; i < MAX_SIZE * MAX_SIZE; i ++) {
            self.svd_buf_in.matrix[i] = i; 
        }
    }
#endif

private:
    svd_token svd_buf_in;

    svd_token svd_buf_out;
    

}; 
#endif 
