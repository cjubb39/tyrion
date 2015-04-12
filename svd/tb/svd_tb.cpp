#include "svd_tb.h"
#include "gm_jacobi.h"

void svd_tb::fill_buf() {
	//Not sure what headers I can use here so 
	//real basic matrix
	int i = 0; 
	for(; i < SVD_INPUT_SIZE(mat_size); i++) {
		input_matrix[i] = i; 
		golden_input_matrix[i] = i;
	}
}

void svd_tb::compute_golden_model(void) {
	gm_jacobi(golden_input_matrix, mat_size,
			SVD_GET_S(golden_matrix, mat_size),
			SVD_GET_U(golden_matrix, mat_size),
			SVD_GET_V(golden_matrix, mat_size));
}

void print_matrix(double *mat, int size) {
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			printf("%f ", mat[i * size + j]);
		}
		printf("\n");
	}
}

void svd_tb::dmac(void) {
	// RESET
	rst_dut.write(0);
	rd_grant.write(0);
	wr_grant.write(0);
	conf_done.write(0);
	bufdin.reset_put();
	bufdout.reset_get();
	wait();

	mat_size = MAX_SIZE;
	fill_buf();
	compute_golden_model();

	rst_dut.write(1);
	conf_size.write(mat_size);
	conf_done.write(1);

	while (true) {
		do { wait(); }
		while (!rd_request.read() && !wr_request.read() && !svd_done.read());

		if (svd_done.read()) {
			cout << "TB: SVD done @ " << sc_time_stamp() << endl;
			// Reset DUT
			rst_dut.write(0);

			for (int i = 0; i < SVD_OUTPUT_SIZE(mat_size); ++i) {
				SVD_CELL_TYPE diff = output_matrix[i] - golden_matrix[i];
				if (diff < 0) {
					diff *= -1;
				}

				mismatches += !(diff < MAX_ERROR);
			}

			/* exit loop now */
			break;
		}

		if (rd_request.read()) {
			unsigned index = rd_index.read();
			unsigned length = rd_length.read();

			rd_grant.write(true);
			do { wait(); }
			while (rd_request.read());
			rd_grant.write(false);
			wait();

			// DMA from memory to DUT
			int i;
			for (i = index; i < index + length; ++i) {
				bufdin.put(input_matrix[i]);
#ifdef VERBOSE
				cout << "TB PUT: (index, length, i, val)" << index << " " << length
					<< " " << i << " " << input_matrix[i] << endl;
#endif
				wait();
			}
		} else if (wr_request.read()) {
			unsigned index = wr_index.read();
			unsigned length = wr_length.read();

			wr_grant.write(true);
			do { wait(); }
			while (wr_request.read());
			wr_grant.write(false);
			wait();

			// DMA from device to memory
			int i;
			for (i = index; i < index + length; ++i) {
				output_matrix[i] = bufdout.get();
#ifdef VERBOSE
				cout << "TB GET: (index, length, i, val)" << index << " " << length
					<< " " << i << " " << input_matrix[i] << endl;
#endif
				wait();
			}
		}
	}

	// Stop simulation
	sc_stop();

#ifdef VERBOSE
	/* print matrices */
	cout << "Input: " << endl;
	print_matrix(input_matrix, mat_size);
	cout << endl;

	cout << "Software: " << endl;
	print_matrix(SVD_GET_S(golden_matrix, mat_size), mat_size);
	print_matrix(SVD_GET_U(golden_matrix, mat_size), mat_size);
	print_matrix(SVD_GET_V(golden_matrix, mat_size), mat_size);
	cout << endl;

	cout << "Hardware: " << endl;
	print_matrix(SVD_GET_S(output_matrix, mat_size), mat_size);
	print_matrix(SVD_GET_U(output_matrix, mat_size), mat_size);
	print_matrix(SVD_GET_V(output_matrix, mat_size), mat_size);
	cout << endl << endl;
#endif

	int errors = get_mismatches();     // show the testing summary
	if (errors > 0)
		cout << "Simulation with " << errors << " mismatches." << endl;
	else
		cout << "Simulation Successful! @ " << sc_time_stamp() << endl;
}

