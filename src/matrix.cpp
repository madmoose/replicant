#include "matrix.h"

matrix_t::matrix_t()
{
	for (int r = 0; r != 3; ++r)
		for (int c = 0; c != 4; ++c)
			m[r][c] = (r == c) ? 2.0 : 1.0;
}

static inline void swap_rows(double *r1, double *r2)
{
	for (int c = 0; c != 8; ++c)
	{
		float t = r1[c];
		r1[c] = r2[c];
		r2[c] = t;
	}
}

static inline void subtract_row(double *r1, double factor, double *r2)
{
	for (int c = 0; c != 8; ++c)
		r1[c] -= factor * r2[c];
}

static inline void divide_row(double *r1, double d)
{
	for (int c = 0; c != 8; ++c)
		r1[c] /= d;
}

#include <cstdio>

void matrix_t::dump()
{
	for (int r = 0; r != 3; ++r)
	{
		for (int c = 0; c != 4; ++c)
		{
			printf("%g ", m[r][c]);
		}
		putchar('\n');
	}
	putchar('\n');
	fflush(0);
}

#if 0

static void dump(double m[3][8])
{
	for (int r = 0; r != 3; ++r)
	{
		for (int c = 0; c != 8; ++c)
		{
			printf("%11.7f ", m[r][c]);
		}
		putchar('\n');
	}
	putchar('\n');
	fflush(0);
}
#endif

void matrix_t::get_inverted_matrix(matrix_t &out)
{
	// Crude manual matrix inversion, but it works! :)

	double w[3][8];

	for (int r = 0; r != 3; ++r)
	{
		for (int c = 0; c != 4; ++c)
		{
			w[r][c] = m[r][c];
			w[r][c+4] = (r == c) ? 1.0 : 0.0;
		}
	}

	if (w[0][0] == 0.0)
	{
		if (w[1][0] != 0.0)
			swap_rows(w[0], w[1]);
		else
			swap_rows(w[0], w[2]);
	}
	divide_row(w[0], w[0][0]);
	subtract_row(w[1], w[1][0], w[0]);
	subtract_row(w[2], w[2][0], w[0]);

	if (w[1][1] == 0.0)
		swap_rows(w[1], w[2]);

	divide_row(w[1], w[1][1]);
	subtract_row(w[0], w[0][1], w[1]);
	subtract_row(w[2], w[2][1], w[1]);

	divide_row(w[2], w[2][2]);
	subtract_row(w[0], w[0][2], w[2]);
	subtract_row(w[1], w[1][2], w[2]);

	for (int r = 0; r != 3; ++r)
	{
		w[r][7] = -w[r][3];
		w[r][3] = 0.0;
	}

	for (int r = 0; r != 3; ++r)
		for (int c = 0; c != 4; ++c)
			out.m[r][c] = w[r][c+4];
}
