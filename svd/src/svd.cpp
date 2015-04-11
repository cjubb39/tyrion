#include "svd.h"
#include "jacobi.h"

void svd::config_svd()
{
	// Initialization
	size.write(0);
	init_done.write(false);

	wait();

	// Read configuration until done
	bool done = false;
CONFIG_REGISTER_WHILE:
	do {
		wait();
		done = conf_done.read();
		int a = conf_size.read();
		size.write(a);
	} while (!done);

	// Let other threads run then do nothing
	init_done.write(true);
	while (true) {
		wait();
	}
}

void svd::load_input()
{
RESET_LOAD:
	bufdin.reset_get();

	rd_index.write(0);
	rd_length.write(0);
	rd_request.write(false);

	input_done.write(false);

	do {wait();}
	while(!init_done.read());

	const int rows = size.read();
	int index = 0;

LOAD_INPUT_WHILE:
	while(true) {
		if (index == rows * rows)
			// Input complete; wait for reset
			do { wait(); }	while (true);

		int length = rows * rows;

		rd_index.write(index);
		rd_length.write(length);
		index += length;

		// 4-phase handshake
		rd_request.write(true);
		do { wait(); } while(!rd_grant.read());
		rd_request.write(false);
		do { wait(); } while(rd_grant.read());

		for (int i = 0; i < MAX_SIZE; ++i) {
			if (i == rows) break;
			for (int j = 0; j < MAX_SIZE; ++j) {
				if (j == rows) break;
				SVD_CELL_TYPE cell = bufdin.get();
				matrix_in[i * rows + j] = cell;
#ifdef VERBOSE
				cout << "DUT GET: (index, length, i, j, val)" << index << " " << length
					<< " " << i << " " << j << " " << cell << endl;
#endif
				wait();
			}
		}

		// 4-phase handshake
		cout << "DUT process start handshake starting" << endl;
		input_done.write(true);
		do { wait(); } while (!process_start.read());
		input_done.write(false);
		do { wait(); } while (process_start.read());
		cout << "DUT process start handshake complete" << endl;
	}
}

void svd::store_output()
{
RESET_STORE:
	bufdout.reset_put();

	wr_index.write(0);
	wr_request.write(false);
	wr_length.write(0);

	output_start.write(false);
	svd_done.write(false);

	do {wait();}
	while(!init_done.read());

	const int rows = size.read();
	int length = rows * rows;
	int index = 0;

STORE_OUTPUT_WHILE:
	while(true) {

		if (index == NUM_OUTPUT_MATRIX * rows * rows) {
			// DEBAYER Done (need a reset)
			cout << "DUT DEBAYER DONE" << endl;
			svd_done.write(true);
			do { wait(); } while(true);
		}

		// 4-phase handshake
		do { wait(); }
		while (!process_done.read());
		output_start.write(true);
		do { wait(); }
		while (process_done.read());
		output_start.write(false);

		/* S matrix */
		// Send DMA request
		wr_index.write(index);
		wr_length.write(length);
		index += length;

		wr_request.write(true);
		do { wait(); } while(!wr_grant.read());
		wr_request.write(false);
		do { wait(); } while(wr_grant.read());

		for (int i = 0; i < MAX_SIZE; ++i) {
			if (i == rows) break;
			for (int j = 0; j < MAX_SIZE; ++j) {
				if (j == rows) break;
				SVD_CELL_TYPE cell = s[i * size + j];
				bufdout.put(cell);
#ifdef VERBOSE
				cout << "DUT PUT: (index, length, i, j, val)" << index << " " << length
					<< " " << i << " " << j << " " << cell << endl;
#endif
				wait();
			}
		}
		

		/* U matrix */
		// Send DMA request
		wr_index.write(index);
		wr_length.write(length);
		index += length;

		wr_request.write(true);
		do { wait(); } while(!wr_grant.read());
		wr_request.write(false);
		do { wait(); } while(wr_grant.read());

		for (int i = 0; i < MAX_SIZE; ++i) {
			if (i == rows) break;
			for (int j = 0; j < MAX_SIZE; ++j) {
				if (j == rows) break;
				SVD_CELL_TYPE cell = u[i * size + j];
				bufdout.put(cell);
#ifdef VERBOSE
				cout << "DUT PUT: (index, length, i, j, val)" << index << " " << length
					<< " " << i << " " << j << " " << cell << endl;
#endif
				wait();
			}
		}
		

		/* V matrix */
		// Send DMA request
		wr_index.write(index);
		wr_length.write(length);
		index += length;

		wr_request.write(true);
		do { wait(); } while(!wr_grant.read());
		wr_request.write(false);
		do { wait(); } while(wr_grant.read());

		for (int i = 0; i < MAX_SIZE; ++i) {
			if (i == rows) break;
			for (int j = 0; j < MAX_SIZE; ++j) {
				if (j == rows) break;
				SVD_CELL_TYPE cell = v[i * size + j];
				bufdout.put(cell);
#ifdef VERBOSE
				cout << "DUT PUT: (index, length, i, j, val)" << index << " " << length
					<< " " << i << " " << j << " " << cell << endl;
#endif
				wait();
			}
		}
	}
}

void svd::process_svd()
{
	// Reset
	process_start.write(false);
	process_done.write(false);

	// Wait for configuratin
	do { wait(); } while (!init_done.read());

	const int rows = size.read();
	int svd_row = 0;

DEBAYER_WHILE:
	while (true) {

		if (svd_row == rows)
			// Wait for reset
			do { wait(); } while(true);

		// 4-phase handshake
		do { wait(); }
		while (!input_done.read());
		process_start.write(true);
		do { wait(); }
		while (input_done.read());
		process_start.write(false);
		cout << "DUT process start handshake complete: PS" << endl;

		cout << "about to jacobi start" << endl;
		jacobi(matrix_in, rows, s, u, v);
		cout << "jacobi finish" << endl;

		// 4-phase handshake
		process_done.write(true);
		do { wait(); }
		while (!output_start.read());
		process_done.write(false);
		do { wait(); }
		while (output_start.read());

		svd_row += rows;
	}
}

