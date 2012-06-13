#ifndef VEC_H
#define VEC_H

#include "matrix.h"

class vec_t
{
public:
	float x;
	float y;
	float z;

	vec_t()
		: x(0.0), y(0.0), z(0.0)
	{}

	vec_t(float x, float y, float z)
		: x(x), y(y), z(z)
	{}

	void dump();

	void multiply_by_matrix(matrix_t &m);

	void project(int *screen_x, int *screen_y, float camera_angle);
};

#endif
