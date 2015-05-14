#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h> 


#define OFFSET 1023
#define MAX 8388608

void double_to_bv(double d, uint64_t* fixed) { 
    double sub = MAX; 
    *fixed = 0; 
    int i; 
    for (i = 63; i >= 0; i--) {
        if (d > sub) {
        //set the bit and substract the value from sub 
            d -= sub;
            *fixed |= ( 1UL << i); 
        }
        sub /= 2.0; 
    }
}

void convert_to_bv(double* fixed) {
    double d = *fixed; 
    double_to_bv(d, (uint64_t *) fixed); 
}

void bv_to_double(uint64_t fixed, double* d) { 
    double sub = MAX; 
    *d = 0.0; 
    int i; 

    for (i = 63; i >= 0; i--) {
        if ((fixed & (1UL << i))) {
            
            *d += sub;
        }
        sub /= 2.0; 
    }

}


void convert_to_double(uint64_t* d) {
    uint64_t fixed  = *d; 
    bv_to_double(fixed, (double*) d); 
}


int main(int argc, char** argv) {
    srand(time(NULL)); 

    int i;
/* 
    uint64_t res; 
    double random; 
    double resd; 
    double_to_bv((double) 0.0, &res);
    bv_to_double(res, &resd);
    double_to_bv((double) 0.25, &res); 
    bv_to_double(res, &resd);

*/ 
    double random; 

    for (i = 0; i < 10; i++) {
        random = rand()  / 13000.0;
        printf("Double %f internal uint64_t %lu\n", random, (uint64_t) random); 
        convert_to_bv((double*) &random); 

        printf("BV: %lu\n", (uint64_t) random); 
        uint64_t* ptr = (uint64_t*) &random; 

        printf("BV: %lu\n", *ptr); 
        convert_to_double((uint64_t*) &random);  
        printf("Back to double %f internal %lu \n", random, (uint64_t) random); 
    }
}


