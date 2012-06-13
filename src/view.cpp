#include "view.h"

#include "reader.h"

#include <cstdio>
#include <cmath>

#include <cstdlib>

void view_t::read(reader_t *r)
{
	r->read_le32(&frame);

	for (int row = 0; row != 3; ++row)
		for (int col = 0; col != 4; ++col)
			r->read_float(&view_matrix.m[row][col]);

	r->read_float(&fovx);

	/*
		printf("fovx: %g  ->  ", fovx);
		view_matrix.dump();
	*/

	matrix_t inv_matrix;
	view_matrix.get_inverted_matrix(inv_matrix);
	//inv_matrix.dump();
}

void view_t::dump()
{
	printf("%d %f\n", frame, fovx);
	view_matrix.dump();
}
