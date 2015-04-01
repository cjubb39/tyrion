#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <Eigen/Core>
#include <Eigen/SVD>

using namespace std;
using namespace Eigen;

void test_svd(MatrixXd m) {
	JacobiSVD<MatrixXd> svd(m, ComputeThinU | ComputeThinV);
#if 0
	cout << "Here is the matrix m:" << endl << m << endl;
#endif
	cout << svd.singularValues() << endl;
#if 0
	cout << "Its left singular vectors are the columns of the thin U matrix:" <<
		endl << svd.matrixU() << endl;
	cout << "Its right singular vectors are the columns of the thin V matrix:"
		<< endl << svd.matrixV() << endl;
#endif
}

int main() {
	int n;

	typedef vector<vector<double> > Rows;
	Rows rows;
	ifstream input("input-matrix.txt");
	char const row_delim = '\n';
	char const field_delim = '\t';

	input >> n;

	double **r;
	int i, j;


	r = (double **) malloc(sizeof *r * n);
	for (i = 0; i < n; ++i)
		r[i] = (double *) malloc(sizeof **r * n);

	i = -1; j = 0;

	for (string row; getline(input, row, row_delim); ++i) {
		rows.push_back(Rows::value_type());
		istringstream ss(row);
		j = 0;
		for (string field; getline(ss, field, field_delim);++j ) {
			double tmp = stod(field);
			r[i][j] = tmp;
			rows.back().push_back(tmp);
		}
	}

	MatrixXd m(n,n);

	for (i = 0; i < n; ++i) {
		for (j = 0; j < n; ++j) {
			m(i,j) = r[i][j];
		}
	}

	test_svd(m);
	return 0;
}
