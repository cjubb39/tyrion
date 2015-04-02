#include "svd.h"

#include "jacobi.h"

svd::beh(void) {
	/* reset behavior */
	data_in_grant.write(false);
	data_out_req.write(false);

	wait();

	/* let's roll */
	while (true) {
		do {wait();}
		while (!data_in_req.read());
		data_in_grant.write(true);
		matrix_in = data_in.read();
		do {wait();}
		while (data_in_req.read());
		data_in_grant.write(false);

		/* call to jacobi */
		wait();
		jacobi(matrix_in.matrix, matrix_in.size, s.matrix, u.matrix, v.matrix);
		wait();

		/* send U back */
		data_out_req.write(true);
		data_out.write(u);
		do {wait();}
		while (!data_out_grant.read());
		data_out_req.write(false);
		do {wait();}
		while(data_out_grant.read());

		/* send S back */
		data_out_req.write(true);
		data_out.write(s);
		do {wait();}
		while (!data_out_grant.read());
		data_out_req.write(false);
		do {wait();}
		while(data_out_grant.read());

		/* send V back */
		data_out_req.write(true);
		data_out.write(v);
		do {wait();}
		while (!data_out_grant.read());
		data_out_req.write(false);
		do {wait();}
		while(data_out_grant.read());
}
