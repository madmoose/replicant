#include "vec.h"

#include <cmath>

#include <cstdio>

void vec_t::dump()
{
	printf("vec: %g %g %g\n", x, y, z);
}

void vec_t::multiply_by_matrix(matrix_t &m)
{
	vec_t r;

	r.x = x*m(0,0) + y*m(0,1) + z*m(0,2) + m(0,3);
	r.y = x*m(1,0) + y*m(1,1) + z*m(1,2) + m(1,3);
	r.z = x*m(2,0) + y*m(2,1) + z*m(2,2) + m(2,3);

	x = r.x;
	y = r.y;
	z = r.z;
}

void vec_t::project(int *screen_x, int *screen_y, float fovx)
{
	double f = 1.0 / tan(fovx / 2.0);
	double aspect_ratio = 640.0 / 480.0;

	double x0 = f * x / z;
	double y0 = f * aspect_ratio * y / z;

	x0 = 320.0 * (1.0 + x0);
	y0 = 240.0 * (1.0 + y0);

	*screen_x = 640 - (int)x0;
	*screen_y = 480 - (int)y0;
}
