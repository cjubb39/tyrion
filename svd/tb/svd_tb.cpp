#include "svd_tb.h"
#include <stdio.h>

void svd_tb::print_buf(svd_token s) {
	int i; 
	for (i = 0; i < s.size * s.size; i++) {
		printf("%d\t", s.matrix[i]);
		if (i && !i%s.size)
			printf("\n");
	}
}	


void svd_tb::fill_buf() {
        //Not sure what headers I can use here so 
        //real basic matrix
        int i = 0;
	svd_buf_in.size = MAX_SIZE; 
        for(; i < MAX_SIZE * MAX_SIZE; i ++) {
            svd_buf_in.matrix[i] = i; 
        }
				svd_buf_in.size = MAX_SIZE;
}

void svd_tb::send(){
    req_in.write(false);
    rst_dut.write(false);

    wait(); 
    rst_dut.write(true);
    
    while(true) {
	svd_tb::fill_buf();
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
	int i = 0;

    grant_out.write(false); 

    wait(); 
    while (true) {
	do {wait(); }
	while(!req_out.read());
 
        grant_out.write(true); 
	wait();
        svd_buf_out = data_out.read();
	do {wait();}
	while (req_out.read()); 
	grant_out.write(false); 

	cout << svd_buf_out << endl;

	if (++i == 3)
		sc_stop();
    }

}



