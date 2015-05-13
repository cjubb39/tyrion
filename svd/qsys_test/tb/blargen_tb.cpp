#include "blargen_tb.h"

void blargen_tb::beh(void) {
	data_to_dut.reset_put();
	data_from_dut.reset_get();
	wait();

	for (int i = 0; i < 4; ++i) {
		for (int j = 0 ; j < 128; ++j) {
			data_to_dut.put(0 * 128 + j);
			wait();
		}
		for (int j = 0 ; j < 128; ++j) {
			unsigned tmp = data_from_dut.get();
			wait();
			cout << "RECIEVED: " << tmp << " @ " << sc_time_stamp() << endl;
		}
	}

	sc_stop();
}
