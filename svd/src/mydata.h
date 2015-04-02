#ifndef _MYDATA_H_
#define _MYDATA_H_

#define MAX_SIZE 25

typedef struct svd_token {
	double matrix[MAX_SIZE * MAX_SIZE]; /* row major form */
	unsigned size; /* of one dimension */
} svd_token_t;

#endif
