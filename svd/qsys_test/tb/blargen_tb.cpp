#include "blargen_tb.h"

void blargen_tb::beh(void) {
	data_to_dut.reset_put();
	data_from_dut.reset_get();
	wait();

	for (int i = 0; i < 32; ++i) {
		data_to_dut.put(i);
		wait();
		unsigned tmp = data_from_dut.get();
		wait();
		cout << "RECIEVED: " << tmp << " @ " << sc_time_stamp() << endl;
	}

	sc_stop();
}
