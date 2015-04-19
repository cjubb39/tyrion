#ifndef _MYDATA_H_
#define _MYDATA_H_

#define SC_INCLUDE_FX
#include <systemc.h>

//#define REAL_FLOAT
//#define SC_FIXED_POINT_FAST
//#define SC_FIXED_POINT
#define CTOS_SC_FIXED_POINT

#define MAX_SIZE 25

#if defined(REAL_FLOAT)
#define SVD_CELL_TYPE double
#elif defined(SC_FIXED_POINT_FAST)
#define SC_FIXED_TYPE sc_dt::sc_fixed_fast
#elif defined(SC_FIXED_POINT)
#define SC_FIXED_TYPE sc_dt::sc_fixed
#elif defined(CTOS_SC_FIXED_POINT)
#include <ctos_fx.h>
#define SC_FIXED_TYPE ctos_sc_dt::sc_fixed
#else
#error "specify fixed or floating point type"
#endif

#ifndef REAL_FLOAT
#define WL 128
#define IWL 32
#define SVD_CELL_TYPE SC_FIXED_TYPE<WL,IWL>
#endif

#define SVD_INPUT_SIZE(__sz) (__sz * __sz)
#define SVD_OUTPUT_SIZE(__sz) (3 * __sz * __sz)
#define SVD_GET_S(__ptr, __sz) (__ptr)
#define SVD_GET_U(__ptr, __sz) (__ptr + __sz * __sz)
#define SVD_GET_V(__ptr, __sz) (__ptr + 2 * __sz * __sz)

#endif
