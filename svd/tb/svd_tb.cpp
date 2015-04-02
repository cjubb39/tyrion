#include "svd_tb.h"

void svd_tb::send(){
#if 0    
    
    req_in.write(false);

    data_in.write(NULL); 

    wait(); 
    fill_buf();
    read_in.write(false); 
    wait(); 
    while (grant_in.read()) {
        read_in.write(true); 
        wait(); 
        data_in.write(svd_buf_in); 
        wait(); 

    }
#endif

}



void svd_tb::recv(){
#if 0

    grant_out.write(false); 

    wait(); 
    while (req_out.read()) {
        grant_out.write(true); 
        wait(); 
        data_out.read(svd_buf_out); 
        wait(); 

    }

#endif
}



