#include "blargen_tb.h"

void blargen_tb::beh(void) {
	data_to_dut.reset_put();
	data_from_dut.reset_get();
	wait();

	for (int i = 0; i < 8; ++i) {
		for (int j = 0 ; j < 4; ++j) {
			data_to_dut.put(i * 4 + j);
			wait();
		}
		for (int j = 0 ; j < 4; ++j) {
			unsigned tmp = data_from_dut.get();
			wait();
			cout << "RECIEVED: " << tmp << " @ " << sc_time_stamp() << endl;
		}
	}

	sc_stop();
}
