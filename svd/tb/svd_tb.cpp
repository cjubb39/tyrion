#include "svd_tb.h"


void svd_tb::fill_buf() {
        //Not sure what headers I can use here so 
        //real basic matrix
        int i = 0; 
        for(; i < MAX_SIZE * MAX_SIZE; i ++) {
            svd_buf_in.matrix[i] = i; 
        }
}
void svd_tb::send(){
    req_in.write(false);

    wait(); 
    
    while(true) {

	svd_tb::fill_buf();
	req_in.write(true);
	wait(); //not sure thats neccessary
	do {wait();} 
	while(!grant_in.read()); 
	 
	data_in.write(svd_buf_in);
	
	wait();
	 
	req_in.write(true); 
	do {wait();} 
	while(grant_in.read()); 
	sc_stop(); // think this is the place to put it 

    }

}



void svd_tb::recv(){

    grant_out.write(false); 

    wait(); 
    while (true) {
	do {wait(); }
	while(!req_out.read());
 
        grant_out.write(true); 
	wait();
        svd_buf_out = data_out.read();
	//print matrix somehow ? 
	do {wait();}
	while (req_out.read()); 
	grant_out.write(false); 

    }

}



