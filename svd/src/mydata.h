#define MAX_SIZE 25

typedef struct svd_token {
	double matrix[MAX_SIZE][MAX_SIZE];
	unsigned size;
} svd_token_t;
