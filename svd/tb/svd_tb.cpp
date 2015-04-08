#include "svd_tb.h"
#include "jacobi.h"

void svd_tb::fill_buf() {
	//Not sure what headers I can use here so 
	//real basic matrix
	int i = 0; 
	for(; i < MAX_SIZE * MAX_SIZE; i ++) {
		svd_buf_in.matrix[i] = i; 
	}
	svd_buf_in.size = MAX_SIZE;
}

void svd_tb::compute_golden_model(void) {
	jacobi(svd_buf_in.matrix, svd_buf_in.size, gm_s.matrix, gm_u.matrix,
			gm_v.matrix);
	gm_u.size = svd_buf_in.size;
	gm_s.size = svd_buf_in.size;
	gm_v.size = svd_buf_in.size;
}

void svd_tb::send(){
	req_in.write(false);
	rst_dut.write(false);

	wait(); 
	rst_dut.write(true);

	while(true) {
		svd_tb::fill_buf();

		/* calculate golden result */
		compute_golden_model();

		req_in.write(true);
		data_in.write(svd_buf_in);
		wait(); //not sure thats neccessary
		do {wait();} 
		while(!grant_in.read()); 

		wait();

		req_in.write(false);
		do {wait();} 
		while(grant_in.read()); 
	}

}



void svd_tb::recv(){
	int i, err_count = 0;

	grant_out.write(false); 

	wait(); 
	for (i = 0; i < 3; ++i) {
		do {wait(); }
		while(!req_out.read());

		grant_out.write(true); 
		wait();
		svd_buf_out = data_out.read();
		do {wait();}
		while (req_out.read()); 
		grant_out.write(false); 

		switch(i) {
			case 0: hw_u = svd_buf_out; break;
			case 1: hw_s = svd_buf_out; break;
			case 2: hw_v = svd_buf_out; break;
		}
	}

	cout << "HW U: " << endl << hw_u << endl;
	cout << "GM U: " << endl << gm_u << endl;
	cout << "HW S: " << endl << hw_s << endl;
	cout << "GM S: " << endl << gm_s << endl;
	cout << "HW V: " << endl << hw_v << endl;
	cout << "GM V: " << endl << gm_v << endl;

	err_count += !(hw_u == gm_u);
	err_count += !(hw_s == gm_s);
	err_count += !(hw_v == gm_v);

	cout << "Errors: " << err_count << endl;

	sc_stop();
}



