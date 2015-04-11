extern "C" {
#include <csp/csp.h>

#include "util.h"
#include "io.h"
}

#include "svd-wrapper.hpp"

void svd_wrapper::ioread32(struct io_req *req, struct io_rsp *rsp) {
	int reg = req->local_offset >> 2;

		obj_dbg(&svd->dev.obj, "%s: size %d offset 0x%x\n", __func__, req->size, req->local_offset);

	switch (reg) {
		case SVD_REG_CMD:
			rsp->val = status_reg;
			break;
		case SVD_REG_SRC:
			rsp->val = dma_phys_addr_src;
			break;
		case SVD_REG_DST:
			rsp->val = dma_phys_addr_dst;
			break;
		case SVD_REG_SIZE:
			rsp->val = conf_size.read();
			break;
		case SVD_REG_MAX_SIZE:
			rsp->val = MAX_SIZE;
			break;
		case SVD_REG_ID:
			rsp->val = svd->dev.id;
			break;
		default:
			BUG();
	}
}

void svd_wrapper::iowrite32(const struct io_req *req, struct io_rsp *rsp)
{
	int reg = req->local_offset >> 2;

		obj_dbg(&svd->dev.obj, "%s: size %d offset 0x%x val 0x%x\n",
						__func__, req->size, req->local_offset, req->val);

	rsp->val = req->val;

	switch (reg) {
		case SVD_REG_CMD:
			cout << "SVD_REG_CMD: " << req->val << "; STATUS REG: " << status_reg << endl;
    	if (req->val == 1) {
      	BUG_ON((status_reg != 0));
				conf_done.write(true);
				start_fifo.put(true);
			}
			else if (req->val != 0) {
		  	BUG();
			}
			status_reg = req->val;
			cout << "SVD REG CMD written" << endl;
			break;
		case SVD_REG_SRC:
			dma_phys_addr_src = req->val;
			break;
		case SVD_REG_DST:
			dma_phys_addr_dst = req->val;
			break;
		case SVD_REG_SIZE:
			cout << "SVD_REG_SIZE write: " << req->val << endl;
			conf_size.write(req->val);
			cout << "SVD_REG_SIZE written" << endl;
			break;
		default:
			BUG();
	}
}

void svd_wrapper::copy_from_dram(u64 index, unsigned length) {
	obj_dbg(&svd->dev.obj, "%s\n", __func__);
	
	cout << "WRAP CFD i, l: " << index << "; " << length << endl;

	/* byte address */
	out_phys_addr.put(dma_phys_addr_src +
			(index * sizeof(SVD_CELL_TYPE /* DMA token */)));
	out_len.put(length);
	out_write.put(false);
	out_start.put(true);
}

void svd_wrapper::copy_to_dram(u64 index, unsigned length) {
	obj_dbg(&svd->dev.obj, "%s\n", __func__);
	
	out_phys_addr.put(dma_phys_addr_dst +
			(index * sizeof(SVD_CELL_TYPE /* DMA token */)));
	out_len.put(length);
	out_write.put(true);
	out_start.put(true);
}

void svd_wrapper::start() {
	rst_dut.write(false);
	wait();
	rst_dut.write(true);

	for (;;) {
		start_fifo.get();
		cout << "start sig from fifo" << endl;
		obj_dbg(&svd->dev.obj, "CTL start\n");
		cout << "about to drive" << endl;
		drive();
		cout << "driven" << endl;
		obj_dbg(&svd->dev.obj, "SVD done\n");
	}
}

void svd_wrapper::drive()
{
	for (;;) {
		do {
			wait();
		} while (!rd_request.read() && !wr_request.read() && !svd_done.read());
		cout << "REQUEST" << endl;
		
		if (svd_done.read()) {
			cout << "SVD DONE" << endl;
			rst_dut.write(false);
			wait();
			rst_dut.write(true);
			// Set bits 5:4 to "10" -> accelerator done
		  status_reg &= ~STATUS_RUN;
			status_reg |= STATUS_DONE;
			device_sync_irq_raise(&svd->dev);
			break;
		}
		if (rd_request.read()) {
			unsigned index = rd_index.read();
			unsigned length = rd_length.read();

			cout << "WRAP RD i, l: " << index << "; " << length << endl;

			rd_tran_cnt++;
			rd_byte += length * sizeof(SVD_CELL_TYPE);

			rd_grant.write(true);

			do { wait(); }
			while (rd_request.read());
			rd_grant.write(false);
			wait();
			
			cout << "about to copy from dram" << endl;
			copy_from_dram((u64) index, length);
			cout << "copied from dram" << endl;

		} else {
			// WRITE REQUEST
			unsigned index = wr_index.read();
			unsigned length = wr_length.read();
			cout << "WRAP WR i, l: " << index << "; " << length << endl;
			wr_tran_cnt++;
			wr_byte += length * sizeof(SVD_CELL_TYPE);

			wr_grant.write(true);

			do { wait(); }
			while (wr_request.read());
			wr_grant.write(false);
			wait();

			cout << "about to copy from dram" << endl;
			copy_to_dram((u64) index, length);
			cout << "copied to dram" << endl;
		}
	}
}

void svd_wrapper::io()
{
	struct io_req req;
	struct io_rsp rsp;

	for (;;) {
		/*
		 * Most of the time the channel will be empty; we speed things
		 * up by just peeking directly at the queue, avoiding syscalls.
		 * We do this every 10 cycles to simulate faster.
		 */
		wait(10);
		if (csp_channel_is_empty(svd->dev.io_req))
			continue;

		if (unlikely(io_recv_req(svd->dev.io_req, &req)))
			die_errno(__func__);

		//BUG_ON(req.size != 4); /* XXX */

		rsp.local_offset = req.local_offset;
		rsp.size = req.size;

		if (req.write){ 
			cout << "IO WRITE req" << endl;
			iowrite32(&req, &rsp);
		}
		else {
			cout << "IO READ req" << endl;
			ioread32(&req, &rsp);
		}
		if (unlikely(io_send_rsp(req.rsp_chan, &rsp)))
			die_errno(__func__);
	}
}
	
