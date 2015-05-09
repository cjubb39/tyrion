#include "svd_tb.h"
#include "gm_jacobi.h"

void svd_tb::fill_buf() {
	//Not sure what headers I can use here so 
	//real basic matrix
	int i = 0; 
	for(; i < SVD_INPUT_SIZE(mat_size); i++) {
		int tmp = rand() % 8192;
		input_matrix[i] = tmp;
		golden_input_matrix[i] = tmp;
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

#ifndef REAL_FLOAT
void print_matrix(SVD_CELL_TYPE *mat, int size) {
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			printf("%f ", mat[i * size + j].to_double());
		}
		printf("\n");
	}
}
#endif

void svd_tb::dmac(void) {
	// RESET
	srand(time(NULL));
	rst_dut.write(0);
#if 0
	rd_grant.write(0);
	wr_grant.write(0);
	conf_done.write(0);
	bufdin.reset_put();
	bufdout.reset_get();
#endif
	data_to_dut.reset_put();
	data_from_dut.reset_get();

	wait();

	mat_size = MAX_SIZE;
	fill_buf();
	compute_golden_model();

	rst_dut.write(1);
#if 0
	conf_size.write(mat_size);
	conf_done.write(1);
#endif

#if 0
	while (true) {
		do { wait(); }
		while (!rd_request.read() && !wr_request.read() && !svd_done.read());

		if (svd_done.read()) {
			cout << "TB: SVD done @ " << sc_time_stamp() << endl;
			// Reset DUT
			rst_dut.write(0);

			for (int i = 0; i < SVD_OUTPUT_SIZE(mat_size); ++i) {
				double diff =
#ifdef REAL_FLOAT
					abs(output_matrix[i])
#else
					abs(output_matrix[i].to_double())
#endif
					- abs(golden_matrix[i]);
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
#endif
	/* put data to DUT */
	for (int i = 0; i < SVD_INPUT_SIZE(MAX_SIZE); ++i) {
		SVD_CELL_TYPE tmp;
		sc_uint<64> tmp_sc_uint;
		uint64_t whole;
		uint32_t lower_half, upper_half;

		tmp = input_matrix[i];
		
		/* TODO check this does what I expect */
		tmp_sc_uint = tmp.range();
		whole = tmp_sc_uint;

		lower_half = whole;
		upper_half = whole >> 32;

		data_to_dut.put(lower_half);
		wait();
		data_to_dut.put(upper_half);
		wait();
	}

	/* get back result */
	for (int i = 0; i < SVD_OUTPUT_SIZE(MAX_SIZE); ++i) {
		SVD_CELL_TYPE tmp;
		sc_uint<32> tmp_sc_uint;
		uint64_t whole;
		uint32_t lower_half, upper_half;

		tmp_sc_uint = data_from_dut.get();
		lower_half = tmp_sc_uint;
		wait();

		tmp_sc_uint = data_from_dut.get();
		upper_half = tmp_sc_uint;
		wait();

		whole = ((uint64_t) upper_half) << 32 + lower_half;
		CTOS_FX_ASSIGN_RANGE(tmp, whole);
		output_matrix[i] = tmp;
	}
	
	do {wait();}
	while (!svd_done.read());

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

