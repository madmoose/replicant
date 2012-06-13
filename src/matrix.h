#ifndef MATRIX_H
#define MATRIX_H

class matrix_t
{
public:
	float m[3][4];

	matrix_t();

	float &operator()(int r, int c) { return m[r][c]; }

	void get_inverted_matrix(matrix_t &out);

	void dump();
};


#endif
