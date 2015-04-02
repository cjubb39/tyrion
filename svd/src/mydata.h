#ifndef _MYDATA_H_
#define _MYDATA_H_

#define MAX_SIZE 25

class svd_token {
	public:
	double matrix[MAX_SIZE * MAX_SIZE]; /* row major form */
	unsigned size; /* of one dimension */

	svd_token() {
		size = 0;
	}

	bool operator== (const svd_token &rhs) const {
		if (size != rhs.size)
			return false;

		int i;
		for (i = 0; i < size * size; ++i)
			if (matrix[i] != rhs.matrix[i])
				return false;

		return true;
	}

	friend void sc_trace(sc_trace_file *tf, const svd_token &st,
			const std::string &NAME) {
	}

	friend ostream& operator<< (ostream &os, svd_token const &st) {
		os << "svd_token put-out not implemented";
		return os;
	}
};

#endif
