#include "blargen.h"

void blargen::beh(void) {
	data_in.reset_get();
	data_out.reset_put();
	wait();

	unsigned tmp;

	while(true) {
		tmp = data_in.get();
		wait();
		data_out.put(tmp);
		wait();
	}
}

#ifdef __CTOS__
SC_MODULE_EXPORT(blargen)
#endif
