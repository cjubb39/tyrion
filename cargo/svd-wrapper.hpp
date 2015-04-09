#ifndef SVD_WRAPPER_HPP
#define SVD_WRAPPER_HPP

extern "C" {
#include "svd-sync.h"
}

#include <flex_channels.hpp>

#include "svd.h"

SC_MODULE(svd_wrapper) {
	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_out<bool> rst_dut;

	/* DMA requests interface */
	sc_in<unsigned>  rd_index;
	sc_in<unsigned>  rd_length;
	sc_in<bool>      rd_request;
	sc_out<bool>     rd_grant;

	sc_in<unsigned>  wr_index;
	sc_in<unsigned>  wr_length;
	sc_in<bool>      wr_request;
	sc_out<bool>     wr_grant;

	/* DMA */
	put_initiator<unsigned long long> out_phys_addr;
	put_initiator<unsigned long>      out_len;
	put_initiator<bool>               out_write;
	put_initiator<bool>               out_start;

	sc_out<unsigned>  conf_size;
	sc_out<bool>      conf_done;

	/* computation complete */
	sc_in<bool> svd_done;

	void iowrite32(const struct io_req *req, struct io_rsp *rsp);
	void ioread32(struct io_req *req, struct io_rsp *rsp);

	void drive();
	void copy_from_dram(u64 index, unsigned length);
	void copy_to_dram(u64 index, unsigned length);

	void io();
	void start();

	typedef svd_wrapper SC_CURRENT_USER_MODULE;
	svd_wrapper(sc_core::sc_module_name, struct svd_sync *svd_) {
		SC_CTHREAD(io, clk.pos());
		reset_signal_is(rst, false);

		SC_CTHREAD(start, clk.pos());
		reset_signal_is(rst, false);

		svd = svd_;
	}

private:
	struct svd_sync *svd; /* driver interface */

	u32 dma_phys_addr_src;
	u32 dma_phys_addr_dst;
	size_t dma_size_src;
	size_t dma_size_dst;

	u32 status_reg; /* [0] go command; [4-5] specified in svd-sync.h */

	tlm_fifo<bool> start_fifo; /* handshake btwn driver and device */

	unsigned rd_tran_cnt;
	unsigned long long rd_byte;
	unsigned wr_tran_cnt;
	unsigned long long wr_byte;
};

#endif /* SVD_WRAPPER_HPP */
