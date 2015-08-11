#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "BinaryNum.h"

using namespace boost::numeric::ublas;
using namespace std;

BinaryNum abs(BinaryNum num) {
	return num;
}

/* Matrix inversion routine.
Uses lu_factorize and lu_substitute in uBLAS to invert a matrix */
template<class T>
bool InvertMatrix(const matrix<T>& input, matrix<T>& inverse)
{
	typedef permutation_matrix<std::size_t> pmatrix;

	// create a working copy of the input
	matrix<T> A(input);


	// create a permutation matrix for the LU-factorization
	pmatrix pm(A.size1());

	// perform LU-factorization
	int res = lu_factorize(A, pm);
	if (res != 0){
		return false;
	}
	// create identity matrix of "inverse"
	inverse.assign(identity_matrix<T> (A.size1()));

	// backsubstitute to get the inverse
	lu_substitute(A, pm, inverse);

	return true;
}

template<class T>
double CalcDeterminant(boost::numeric::ublas::matrix<T> m) {
	assert(
			m.size1() == m.size2()
					&& "Can only calculate the determinant of square matrices");
	permutation_matrix<std::size_t> pivots(m.size1());

	const int is_singular = boost::numeric::ublas::lu_factorize(m, pivots);

	if (is_singular)
		return 0.0;

	double d = 1.0;
	const std::size_t sz = pivots.size();
	for (std::size_t i = 0; i != sz; ++i) {
		if (pivots(i) != i) {
			d *= -1.0;
		}
		d *= m(i, i);
	}
	return d;
}

template<class T>
bool BinaryMatrixProduct(const matrix<T>& factor1, matrix<T>& factor2,
		matrix<T>& output) {
	if (factor1.size2() != factor2.size1() || output.size1() != factor1.size1()
			|| output.size2() != factor2.size2()) {
		return false;
	}
	for (int i = 0; i < factor1.size1(); i++) {
		for (int j = 0; j < factor2.size2(); j++) {
			for (int k = 0; k < factor1.size2(); k++) {
				output(i, j) += factor1(i, k) * factor2(k, j);
			}
			output(i, j) = output(i, j) % 2;
		}
	}
	return true;
}
