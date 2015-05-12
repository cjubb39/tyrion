 
#define SC_INCLUDE_FX
#include "systemc.h"

#include <ctos_flex_channels.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <time.h>
#include <stdint.h> 
#include <ctos_fx.h>

#define SC_FIXED_TYPE ctos_sc_dt::sc_fixed

#define WL 64 
#define IWL 24

#define SC_CELL_TYPE SC_FIXED_TYPE<WL, IWL> 

#define OFFSET 1023
#define MAX 8388608

void double_to_bv(double d, uint64_t * fixed) { 
    double sub = (double) MAX; 
    *fixed = 0; 
    int i; 
    printf ("Double: %f ", d); 
    for (i = 63; i >= 0; i--) {
        if (d >= sub) {
        //set the bit and substract the value from sub 
            d -= sub;
            *fixed |= ( 1UL << i); 
        }
        sub /= 2.0; 
    }

    printf("BV: %lu\n", *fixed); 

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

    printf("BV: %lu  Double: %f \n", fixed, *d); 
}


void print_bits(SC_CELL_TYPE t ) {

    int i; 
    for(i = WL - 1; i >= 0; i--) {
        printf("%d", t[i]); 
        if (i && !(i%4)) {
            printf(" ");
        }
    }
    printf("\n"); 

}

void print_double_bits(uint64_t t) {
    int i; 
    for(i = WL - 1; i >= 0; i--) {
        //printf("%d", ((1UL << i) & t)); 
        ((1UL << i ) & t) ? printf("1") : printf("0"); 
        if (i && !(i%4)) {
            printf(" "); 
    
        }
    }

    printf("\n"); 

}

void sc_fixed_to_bv(SC_CELL_TYPE d, uint64_t * fixed) { 
    *fixed = 0; 
    int i; 
    printf ("sc_fixed double: %f ", d.to_double()); 
    for (i = 63; i >= 0; i--) {
        if(d[i]) 
            *fixed |= (1UL << i ); 
        

    }

    printf("BV: %lu\n", *fixed); 

}

void bv_to_sc_fixed(uint64_t fixed, SC_CELL_TYPE* d) { 
    *d = 0.0; 
    double sub = MAX;
    double res = 0.0; 
    int i; 

    for (i = 63; i >= 0; i--) {
        if ((fixed & (1UL << i))) {
            res += sub; 
        }
        sub /= 2.0; 
    }
    *d = res; 
    printf("BV: %lu  Double: %f \n", fixed, (*d).to_double()); 
}



int main(int argc, char** argv) {
    srand(time(NULL)); 

    int i;
    SC_CELL_TYPE st;
    SC_CELL_TYPE fixed_res; 
    st = 0.25; 
    print_bits(st); 
    double t; 
    uint64_t res;
    
    double random; 
    double resd; 
    double_to_bv((double) 0.0, &res);
    bv_to_double(res, &resd);
    double_to_bv((double) 0.25, &res); 
    print_double_bits(res); 
    bv_to_double(res, &resd);
    for (i = 0; i < 10; i++) {
        t = rand()  / 13000.0; // we only have 24 integer bits   
        st  = rand()  / 13000.0; 
        sc_fixed_to_bv(st, &res); 
        bv_to_sc_fixed(res, &fixed_res);


        print_bits(st); 
        print_double_bits(res);
        print_bits(fixed_res); 
    }
}


