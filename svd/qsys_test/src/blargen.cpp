#include "blargen.h"

void blargen::beh(void) {
	data_in.reset_get();
	data_out.reset_put();
	wait();

	//unsigned tmp;

	while(true) {
		for (int i = 0; i < 4; ++i) {
			data[i] = data_in.get();
			wait();
		}
		for (int i = 0; i < 4; ++i) {
			data_out.put(data[i]);
			wait();
		}
	}
}

#ifdef __CTOS__
SC_MODULE_EXPORT(blargen)
#endif
