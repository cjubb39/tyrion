#ifndef _MYDATA_H_
#define _MYDATA_H_

#define MAX_SIZE 25
#define SVD_CELL_TYPE double

#define SVD_INPUT_SIZE(__sz) (__sz * __sz)
#define SVD_OUTPUT_SIZE(__sz) (3 * __sz * __sz)
#define SVD_GET_S(__ptr, __sz) (__ptr)
#define SVD_GET_U(__ptr, __sz) (__ptr + __sz * __sz)
#define SVD_GET_V(__ptr, __sz) (__ptr + 2 * __sz * __sz)

#define MAX_ERROR (0.001)

#if 0
#define MAX_SIZE 5

class svd_token {
	public:
	double matrix[MAX_SIZE * MAX_SIZE]; /* row major form */
	unsigned size; /* of one dimension */

	svd_token() {
		size = 0;
	}

	svd_token(const svd_token &rhs) {
		size = rhs.size;

		int i, j;
		for (i = 0; i < size; ++i) {
			for (j = 0; j < size; ++j) {
				matrix[i*size + j] = rhs.matrix[i*size+j];
			}
		}
	}

	bool operator== (const svd_token &rhs) const {
		if (size != rhs.size)
			return false;

		int i;
		for (i = 0; i < size * size; ++i)
			if (matrix[i] - rhs.matrix[i] > 1e-3)
				return false;

		return true;
	}

	friend void sc_trace(sc_trace_file *tf, const svd_token &st,
			const std::string &NAME) {
	}

	friend ostream& operator<< (ostream &os, svd_token const &st) {
		unsigned i, j;
		for (i = 0; i < st.size; ++i) {
			for (j = 0; j < st.size; ++j) {
				os << st.matrix[i * st.size + j] << " ";
			}
			os << endl;
		}

		return os;
	}
};
#endif

#endif
