#include <systemc.h>
#include <ctos_flex_channels.h>

#include <iostream>
#include <iomanip>
#include <ctime>
#include <ostream>


#ifndef __SVDTB_H__
#define __SVDTB_H__

SC_MODULE(svd_tb) {
    sc_in<bool> clk; 
    sc_in<bool> rst;
    sc_in<bool> rst_dut;

    sc_out<unsigned> data_in; 
    sc_out<bool> req_in; 
    sc_in<bool> grant_in; 

    sc_in<unsigned> data_out; 
    sc_in<bool> req_out; 
    sc_out<bool> grant_out; 


    SC_CTOR(svd_tb) 
        : clk("clk")
        , data_in("data_in")
        , req_in("req_in")
        , grant_in("grant_in")
        , data_out("data_out")
        , req_out("req_out")
        , grant_out("grant_out")

    {
        SC_CTHREAD(send); 
        SC_CTHREAD(recv); 

    }


private:
    

}; 
#endif 
