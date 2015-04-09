extern "C" {
#include <sys/syscall.h>
#include <sys/types.h>
#include <csp/csp.h>

#include "svd-sync.h"
#include "sync.h"
}

#include <flex_channels.hpp>
#include "svd-wrapper.hpp"
#include "svd.h"

#include <sc_dma_controller.hpp>

void svd_main(struct device *dev) {
	struct svd_sync *svd_dev = dev_to_svd(dev);
	sc_clock clk("clk", 1, SC_NS);
	sc_signal<bool> rst("rst");
	sc_signal<bool> rst_dut("rst_dut");

	sc_signal<unsigned>  rd_index;
	sc_signal<unsigned>  rd_length;
	sc_signal<bool>      rd_request;
	sc_signal<bool>      rd_grant;

	sc_signal<unsigned>  wr_index;
	sc_signal<unsigned>  wr_length;
	sc_signal<bool>      wr_request;
	sc_signal<bool>      wr_grant;

	/* DMA */
	put_get_channel<unsigned long long> dma_phys_addr;
	put_get_channel<unsigned long> dma_len;
	put_get_channel<bool> dma_write;
	put_get_channel<bool> dma_start;

	/* Debayer */
	put_get_channel<SVD_CELL_TYPE>  bufdin;
	put_get_channel<SVD_CELL_TYPE>  bufdout;
	sc_signal<unsigned>  conf_size;
	sc_signal<bool>      conf_done;

	/* computation complete */
	sc_signal<bool> svd_done;


	/* modules! */
	svd_wrapper wrapper("wrapper", svd_dev);
	svd dut("dut");
	sc_dma_controller<SVD_CELL_TYPE> dma("dma_controller", &dev->dma_cont);
	int budget, budget_on_loan = 0;

	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);


	/* wire it up */
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
  dut.bufdout(bufdout);
  dut.bufdin(bufdin);
  dut.conf_size(conf_size);
  dut.conf_done(conf_done);
  dut.svd_done(svd_done);

  wrapper.clk(clk);
  wrapper.rst(rst);
  wrapper.rst_dut(rst_dut);
  wrapper.rd_index(rd_index);
  wrapper.rd_length(rd_length);
  wrapper.rd_request(rd_request);
  wrapper.rd_grant(rd_grant);
  wrapper.wr_index(wr_index);
  wrapper.wr_length(wr_length);
  wrapper.wr_request(wr_request);
  wrapper.wr_grant(wr_grant);
  wrapper.conf_size(conf_size);
  wrapper.conf_done(conf_done);
  wrapper.svd_done(svd_done);
  wrapper.out_phys_addr(dma_phys_addr);
  wrapper.out_len(dma_len);
  wrapper.out_write(dma_write);
  wrapper.out_start(dma_start);

  dma.clk(clk);
  dma.rst(rst);
  dma.in_phys_addr(dma_phys_addr);
  dma.in_len(dma_len);
  dma.in_write(dma_write);
  dma.in_start(dma_start);
  dma.in_data(bufdout);
  dma.out_data(bufdin);

	/* simulation time */
	rst.write(false);
	sc_start(10, SC_NS);
	rst.write(true);

	for (;;) {
		if (unlikely(csp_recv_int(dev->sync, &budget)))
			die_errno("%s: recv, __func__");
		budget = sync_budget_update(budget, &budget_on_loan);
		if (!budget) continue;
		sc_start(budget, SC_NS);
	}
}

