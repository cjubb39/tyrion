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

void svd_main(struct device *dev)
{
  struct svd_sync *svd_dev = dev_to_svd(dev);
  sc_clock         clk("clk", 1, SC_NS);
  sc_signal<bool>  rst("rst");
  sc_signal<bool>  rst_dut("rst_dut");

  sc_signal<unsigned> rd_index;   // array index (offset from starting address)
  sc_signal<unsigned> rd_length;  // burst length
  sc_signal<bool>     rd_request; // transaction request
  sc_signal<bool>     rd_grant;   // transaction grant

  sc_signal<unsigned> wr_index;   // array index (offset from starting address)
  sc_signal<unsigned> wr_length;  // burst length
  sc_signal<bool>     wr_request; // transaction request
  sc_signal<bool>     wr_grant;   // transaction grant

  /* DMA controller */
  put_get_channel<unsigned long long> dma_phys_addr;
  put_get_channel<unsigned long>      dma_len;
  put_get_channel<bool>               dma_write;
  put_get_channel<bool>               dma_start;
  put_get_channel<svd_token>          dma_in_data;
  put_get_channel<svd_token>          dma_out_data;

  /* FLASH */
#if 0
  sc_signal<bool>           sched_req;
  sc_signal<bool>           sched_grant;
  sc_signal<flash_pid_t>    next_process;
  sc_signal<bool>           tick_req;
  sc_signal<bool>           tick_grant;
  sc_signal<bool>           change_req;
  sc_signal<bool>           change_grant;
  sc_signal<flash_change_t> change_type;
  sc_signal<flash_pid_t>    change_pid;
  sc_signal<flash_pri_t>    change_pri;
  sc_signal<flash_state_t>  change_state;
#endif

	sc_signal<bool>       data_in_req;
	sc_signal<bool>       data_in_grant;
	sc_signal<svd_token>  data_in;

	sc_signal<bool>       data_out_req;
	sc_signal<bool>       data_out_grant;
	sc_signal<svd_token>  data_out;

  svd_wrapper                   wrapper("wrapper", svd_dev);
  svd                           dut("dut");
  sc_dma_controller<svd_token>  dma("dma_controller", &dev->dma_cont);
  int                           budget_on_loan = 0;
  int                           budget;

  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);

  /* wire everything */
  dut.clk(clk);
  dut.rst(rst_dut);
#if 0
  dut.sched_req(sched_req);
  dut.sched_grant(sched_grant);
  dut.next_process(next_process);
  dut.tick_req(tick_req);
  dut.tick_grant(tick_grant);
  dut.change_req(change_req);
  dut.change_grant(change_grant);
  dut.change_type(change_type);
  dut.change_pid(change_pid);
  dut.change_pri(change_pri);
  dut.change_state(change_state);
#endif
	dut.data_in_req(data_in_req);
	dut.data_in_grant(data_in_grant);
	dut.data_in(data_in);
	dut.data_out_req(data_out_req);
	dut.data_out_grant(data_out_grant);
	dut.data_out(data_out);

  wrapper.clk(clk);
  wrapper.rst(rst);
  wrapper.rst_dut(rst_dut);
#if 0
  wrapper.sched_req(sched_req);
  wrapper.sched_grant(sched_grant);
  wrapper.next_process(next_process);
  wrapper.tick_req(tick_req);
  wrapper.tick_grant(tick_grant);
  wrapper.change_req(change_req);
  wrapper.change_grant(change_grant);
  wrapper.change_pid(change_pid);
  wrapper.change_pri(change_pri);
  wrapper.change_state(change_state);
#endif
	wrapper.data_in_req(data_in_req);
	wrapper.data_in_grant(data_in_grant);
	wrapper.data_in(data_in);
	wrapper.data_out_req(data_out_req);
	wrapper.data_out_grant(data_out_grant);
	wrapper.data_out(data_out);

  wrapper.rd_index(rd_index);   // array index
  wrapper.rd_length(rd_length);
  wrapper.rd_request(rd_request); // transaction request
  wrapper.rd_grant(rd_grant);   // transaction grant
  wrapper.wr_index(wr_index);   // array index
  wrapper.wr_length(wr_length);
  wrapper.wr_request(wr_request); // transaction request
  wrapper.wr_grant(wr_grant);   // transaction grant
  wrapper.out_phys_addr(dma_phys_addr);
  wrapper.out_len(dma_len);
  wrapper.out_write(dma_write);
  wrapper.out_start(dma_start);
  wrapper.from_dma(dma_out_data);
  wrapper.to_dma(dma_in_data);

  dma.clk(clk);
  dma.rst(rst);
  dma.in_phys_addr(dma_phys_addr);
  dma.in_len(dma_len);
  dma.in_write(dma_write);
  dma.in_start(dma_start);
  dma.in_data(dma_in_data);
  dma.out_data(dma_out_data);

  /* run simulation */
  rst.write(false);
  sc_start(10, SC_NS);
  rst.write(true);

  for (;;) {
    if (unlikely(csp_recv_int(dev->sync, &budget)))
      die_errno("%s: recv", __func__);
    budget = sync_budget_update(budget, &budget_on_loan);
    if (!budget)
      continue;
    sc_start(budget, SC_NS);
  }
}
